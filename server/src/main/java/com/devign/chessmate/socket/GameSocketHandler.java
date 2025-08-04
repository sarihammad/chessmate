package com.devign.chessmate.socket;

import com.devign.chessmate.service.MatchmakingService;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.*;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.util.Map;
import com.fasterxml.jackson.core.type.TypeReference;


@Component
public class GameSocketHandler extends TextWebSocketHandler {

    private final MatchmakingService matchmakingService;
    private final ObjectMapper objectMapper = new ObjectMapper();

    public GameSocketHandler(MatchmakingService matchmakingService) {
        this.matchmakingService = matchmakingService;
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        System.out.println("Connection established: " + session.getId());
    }
    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        Map<String, Object> payload = objectMapper.readValue(
            message.getPayload(),
            new TypeReference<Map<String, Object>>() {}
        );
        matchmakingService.handleMessage(session, payload);
    }
    

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
        System.out.println("Connection closed: " + session.getId());
        matchmakingService.removePlayer(session);
    }
}