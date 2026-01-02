@echo off
echo ============================================
echo   PROXY SERVER AUTOMATED TEST SUITE
echo ============================================

echo [TEST 1] Successful Forwarding (HTTP GET)...
curl -I -x http://localhost:8888 http://www.google.com [cite: 97, 98]
echo.

echo [TEST 2] Blocking Blacklisted Domain...
:: Ensure 'example.com' is inside config/blocked.txt [cite: 103]
curl -I -x http://localhost:8888 http://example.com
echo.

echo [TEST 3] Handling Malformed Request...
:: We use 'powershell' to send a raw, broken string to the port 
powershell -Command "$c = New-Object System.Net.Sockets.TCPClient('localhost', 8888); $s = $c.GetStream(); $w = New-Object System.IO.StreamWriter($s); $w.Write('NOT_A_VALID_HTTP_REQUEST\r\n\r\n'); $w.Flush(); $c.Close()"
echo Sent malformed data. Check proxy console for error handling. [cite: 108]
echo.

echo [TEST 4] Concurrency Test...
:: Start multiple curls in the background [cite: 101]
start /b curl -s -o NUL -x http://localhost:8888 http://www.google.com
start /b curl -s -o NUL -x http://localhost:8888 http://www.bing.com
echo Multiple concurrent requests dispatched.
echo.

echo Testing complete. Review 'logs/proxy.log' for proof of actions. [cite: 107]
pause