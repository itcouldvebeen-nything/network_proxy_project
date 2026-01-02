@echo off
echo Starting 20 concurrent requests to Proxy on port 8888...
for /L %%i in (1,1,20) do (
   start /B curl -s -o NUL -I -x http://localhost:8888 http://www.google.com
)
echo.
echo 20 requests sent! 
echo Check your Proxy Console for [FORWARD] and [DONE] messages.
echo Check proxy.log in your Release folder for 20 new entries.
pause