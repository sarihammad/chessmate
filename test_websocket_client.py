import asyncio
import websockets
import json

uri = "ws://localhost:8080/game"

async def test_client():
    async with websockets.connect(uri) as websocket:
        print("[CLIENT] Connected to server")

        # join the game queue
        join_payload = {
            "type": "join",
            "playerId": "test123"
        }
        await websocket.send(json.dumps(join_payload))
        print("[CLIENT] Sent join request")

        while True:
            response = await websocket.recv()
            message = json.loads(response)
            print("[SERVER]", message)

            if message["type"] == "start":
                print("[CLIENT] Match started with opponent:", message["opponentId"])

                # send a move
                move_payload = {
                    "type": "move",
                    "x": 1,
                    "y": 2
                }
                await websocket.send(json.dumps(move_payload))
                print("[CLIENT] Sent move:", move_payload)

            elif message["type"] == "opponentMove":
                print("[CLIENT] Received opponent move:", message)

            elif message["type"] == "gameOver":
                print("[CLIENT] Game Over:", message)
                break

# run test
asyncio.run(test_client())