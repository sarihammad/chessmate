package com.devign.chessmate.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.stereotype.Service;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

@Service
public class MatchmakingService {

    private final ObjectMapper objectMapper = new ObjectMapper();
    private final Queue<WebSocketSession> waitingPlayers = new ConcurrentLinkedQueue<>();
    private final Map<String, WebSocketSession> activeSessions = new ConcurrentHashMap<>();
    private final Map<String, String> matches = new ConcurrentHashMap<>();

    public void handleMessage(WebSocketSession session, Map<String, Object> payload) throws Exception {
        String type = (String) payload.get("type");

        switch (type) {
            case "join" -> joinQueue(session);
            case "move" -> forwardMove(session, payload);
        }
    }

    private void joinQueue(WebSocketSession session) throws Exception {
        WebSocketSession opponent = waitingPlayers.poll();
        if (opponent == null) {
            waitingPlayers.add(session);
        } else {
            String player1Id = session.getId();
            String player2Id = opponent.getId();

            matches.put(player1Id, player2Id);
            matches.put(player2Id, player1Id);

            activeSessions.put(player1Id, session);
            activeSessions.put(player2Id, opponent);

            session.sendMessage(new TextMessage("{\"type\":\"start\",\"color\":\"black\",\"opponentId\":\"" + player2Id + "\"}"));
            opponent.sendMessage(new TextMessage("{\"type\":\"start\",\"color\":\"white\",\"opponentId\":\"" + player1Id + "\"}"));
        }
    }

    private void forwardMove(WebSocketSession session, Map<String, Object> payload) throws Exception {
        String senderId = session.getId();
        String receiverId = matches.get(senderId);
        if (receiverId != null && activeSessions.containsKey(receiverId)) {
            WebSocketSession receiver = activeSessions.get(receiverId);
            payload.put("type", "opponentMove");
            receiver.sendMessage(new TextMessage(objectMapper.writeValueAsString(payload)));
        }
    }

    public void removePlayer(WebSocketSession session) {
        String sessionId = session.getId();
        waitingPlayers.remove(session);
        String opponentId = matches.remove(sessionId);
        if (opponentId != null) {
            matches.remove(opponentId);
            WebSocketSession opponent = activeSessions.remove(opponentId);
            if (opponent != null && opponent.isOpen()) {
                try {
                    opponent.sendMessage(new TextMessage("{\"type\":\"gameOver\",\"winner\":\"you\"}"));
                    opponent.close();
                } catch (Exception ignored) {}
            }
        }
        activeSessions.remove(sessionId);
    }
}