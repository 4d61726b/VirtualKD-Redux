makecert -r -pe -n "CN=VirtualKD-Redux CA" -ss CA -sr LocalMachine -a sha256 -len 4096 -cy authority -sky signature -sv Certs\VKD-Redux_CA.pvk Certs\VKD-Redux_CA.cer
certutil -addstore Root Certs\VKD-Redux_CA.cer
makecert -pe -n "CN=VirtualKD-Redux SPC" -eku "1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13" -a sha256 -len 4096 -cy end -sky signature -ic Certs\VKD-Redux_CA.cer -iv Certs\VKD-Redux_CA.pvk -sv Certs\VKD-Redux_SPC.pvk Certs\VKD-Redux_SPC.cer
pvk2pfx -pvk Certs\VKD-Redux_SPC.pvk -spc Certs\VKD-Redux_SPC.cer -pfx Certs\VKD-Redux_SPC.pfx