@echo off
SETLOCAL EnableDelayedExpansion

:: Configuration
SET PROXY_HOST=localhost
SET PROXY_PORT=8888
SET PROXY_ADDR=%PROXY_HOST%:%PROXY_PORT%

echo ============================================================
echo      PROXY SERVER PROFESSIONAL TEST SUITE
echo ============================================================
echo Target Proxy: %PROXY_ADDR%
echo.

:: 1. Ensure logs directory exists
if not exist "logs" mkdir logs

:: [TEST 1] Successful Forwarding (HTTP GET)
echo [TEST 1] Testing HTTP GET Forwarding...
curl -s -I -x %PROXY_ADDR% http://www.google.com | findstr "HTTP/1.1 200"
if %ERRORLEVEL% EQU 0 (echo [PASS] Google HTTP reachable.) else (echo [FAIL] Check proxy connectivity.)
echo.

:: [TEST 2] HTTPS Tunneling (CONNECT)
echo [TEST 2] Testing HTTPS CONNECT Tunneling...
curl -s -I -x %PROXY_ADDR% https://www.wikipedia.org | findstr "HTTP/1.1 200"
if %ERRORLEVEL% EQU 0 (echo [PASS] HTTPS Tunnel established.) else (echo [FAIL] CONNECT method failed.)
echo.

:: [TEST 3] Blocking Blacklisted Domain
echo [TEST 3] Testing Domain Filtering (example.com)...
:: Note: This assumes example.com is in your blocked.txt
curl -s -I -x %PROXY_ADDR% http://example.com | findstr "403"
if %ERRORLEVEL% EQU 0 (echo [PASS] Domain blocked correctly.) else (echo [FAIL] Filtering logic bypassed.)
echo.

:: [TEST 4] Handling Malformed Request
echo [TEST 4] Sending Malformed Raw Data...
powershell -Command "$c = New-Object System.Net.Sockets.TCPClient('%PROXY_HOST%', %PROXY_PORT%); $s = $c.GetStream(); $w = New-Object System.IO.StreamWriter($s); $w.Write('NOT_A_VALID_HTTP_REQUEST\r\n\r\n'); $w.Flush(); $c.Close()"
echo [INFO] Malformed packet sent. Check console for "HttpRequest error" or graceful closure.
echo.

:: [TEST 5] Concurrency Stress Test
echo [TEST 5] Dispatching 5 Concurrent Background Requests...
for /L %%i in (1,1,5) do (
    start /b curl -s -o NUL -x %PROXY_ADDR% http://www.bing.com
)
echo [INFO] Requests dispatched. Monitor the proxy console for simultaneous [FORWARD] logs.
echo.

echo ============================================================
echo TESTING COMPLETE
echo Please check 'logs/proxy.log' for detailed audit trails.
echo ============================================================
pause