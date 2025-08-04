package com.devign.chessmate;

import com.devign.chessmate.service.MatchmakingService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.TestPropertySource;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;

import java.util.HashMap;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@SpringBootTest
@TestPropertySource(properties = {
    "spring.websocket.max-text-message-size=8192",
    "spring.websocket.max-binary-message-size=8192"
})
class MatchmakingServiceTest {

    private MatchmakingService matchmakingService;

    @BeforeEach
    void setUp() {
        matchmakingService = new MatchmakingService();
    }

    @Test
    void testJoinQueueMessage() throws Exception {
        // Test handling join queue message
        WebSocketSession session = mock(WebSocketSession.class);
        when(session.getId()).thenReturn("test-session-1");
        when(session.isOpen()).thenReturn(true);
        
        Map<String, Object> payload = new HashMap<>();
        payload.put("type", "join");
        
        // Should not throw exception
        assertDoesNotThrow(() -> matchmakingService.handleMessage(session, payload));
    }

    @Test
    void testMoveMessage() throws Exception {
        // Test handling move message
        WebSocketSession session = mock(WebSocketSession.class);
        when(session.getId()).thenReturn("test-session-2");
        when(session.isOpen()).thenReturn(true);
        
        Map<String, Object> payload = new HashMap<>();
        payload.put("type", "move");
        payload.put("from", Map.of("row", 6, "col", 4));
        payload.put("to", Map.of("row", 4, "col", 4));
        
        // Should not throw exception
        assertDoesNotThrow(() -> matchmakingService.handleMessage(session, payload));
    }

    @Test
    void testRemovePlayer() throws Exception {
        // Test removing a player from the service
        WebSocketSession session = mock(WebSocketSession.class);
        when(session.getId()).thenReturn("test-session-3");
        when(session.isOpen()).thenReturn(true);
        
        // Should not throw exception
        assertDoesNotThrow(() -> matchmakingService.removePlayer(session));
    }

    @Test
    void testPlayerMatching() throws Exception {
        // Test that two players can be matched together
        WebSocketSession player1 = mock(WebSocketSession.class);
        WebSocketSession player2 = mock(WebSocketSession.class);
        
        when(player1.getId()).thenReturn("player-1");
        when(player2.getId()).thenReturn("player-2");
        when(player1.isOpen()).thenReturn(true);
        when(player2.isOpen()).thenReturn(true);
        doNothing().when(player1).sendMessage(any(TextMessage.class));
        doNothing().when(player2).sendMessage(any(TextMessage.class));
        
        Map<String, Object> joinPayload = new HashMap<>();
        joinPayload.put("type", "join");
        
        // First player joins (should be added to queue)
        matchmakingService.handleMessage(player1, joinPayload);
        
        // Second player joins (should match with first player)
        matchmakingService.handleMessage(player2, joinPayload);
        
        // Verify that both players received start messages
        verify(player1, times(1)).sendMessage(any(TextMessage.class));
        verify(player2, times(1)).sendMessage(any(TextMessage.class));
    }

    @Test
    void testServiceInitialization() {
        // Test that the service can be initialized properly
        assertNotNull(matchmakingService, "MatchmakingService should be initialized");
    }
} 