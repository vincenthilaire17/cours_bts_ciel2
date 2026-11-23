with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

import re
headings = re.findall(r'<h[1-4][^>]*>[\s\S]*?</h[1-4]>', text)
for h in headings:
    print(h)
