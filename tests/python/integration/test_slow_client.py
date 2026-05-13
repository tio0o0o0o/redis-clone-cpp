import subprocess 
import time
from python.client import RedisClient

def test_slow_client_does_not_block_client():
    server = None
    normal_client = None
    slow_client = None

    try:
        subprocess.run(["pkill", "-f", "redis_clone_server"])

        server = subprocess.Popen(["./build/redis_clone_server"])
        time.sleep(2)

        # Slow client
        slow_client = RedisClient()
        slow_client.connect("127.0.0.1", 6969)
        slow_client.sock.sendall("*3\r\n".encode())

        # Normal client
        normal_client = RedisClient()
        normal_client.connect("127.0.0.1", 6969)
        start = time.monotonic()
        normal_client.send_command("set", "age", "67")
        normal_client.send_command("get", "age")
        res = normal_client.read_response()
        res_time = time.monotonic() - start
        assert res_time < 2
        assert res == "OK"
        res = normal_client.read_response()
        assert res == "67"
    finally:
        if (server):
            server.terminate()
        if (slow_client):
            if (slow_client.sock):
                slow_client.close()
        if (normal_client):
            if (normal_client.sock):
                normal_client.close()
        server.wait()
