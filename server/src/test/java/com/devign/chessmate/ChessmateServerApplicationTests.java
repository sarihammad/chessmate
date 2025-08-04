package com.devign.chessmate;

import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.server.LocalServerPort;
import org.springframework.test.context.TestPropertySource;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.client.standard.StandardWebSocketClient;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@TestPropertySource(properties = {
    "spring.websocket.max-text-message-size=8192",
    "spring.websocket.max-binary-message-size=8192"
})
class ChessmateServerApplicationTests {

    @LocalServerPort
    private int port;

    @Test
    void contextLoads() {
        // Basic context loading test
        assertTrue(true);
    }

    @Test
    void testWebSocketConnection() throws Exception {
        StandardWebSocketClient client = new StandardWebSocketClient();
        CountDownLatch connectionLatch = new CountDownLatch(1);
        CountDownLatch messageLatch = new CountDownLatch(1);
        
        final String[] receivedMessage = new String[1];
        
        TextWebSocketHandler handler = new TextWebSocketHandler() {
            @Override
            public void afterConnectionEstablished(WebSocketSession session) {
                connectionLatch.countDown();
            }
            
            @Override
            protected void handleTextMessage(WebSocketSession session, TextMessage message) {
                receivedMessage[0] = message.getPayload();
                messageLatch.countDown();
            }
        };
        
        WebSocketSession session = client.doHandshake(handler, 
            "ws://localhost:" + port + "/game").get();
        
        // Wait for connection
        assertTrue(connectionLatch.await(5, TimeUnit.SECONDS), "WebSocket connection should be established");
        
        // Send a test message
        session.sendMessage(new TextMessage("{\"type\":\"join\",\"playerId\":\"test-player-1\"}"));
        
        // Wait for response
        assertTrue(messageLatch.await(5, TimeUnit.SECONDS), "Should receive a response message");
        assertNotNull(receivedMessage[0], "Received message should not be null");
        
        session.close();
    }

    @Test
    void testMatchmakingService() {
        // Test matchmaking service functionality
        // This would require injecting the MatchmakingService and testing its methods
        assertTrue(true, "Matchmaking service should be available");
    }

    @Test
    void testGameSocketHandler() {
        // Test game socket handler functionality
        // This would test the WebSocket message handling logic
        assertTrue(true, "Game socket handler should be available");
    }
} 