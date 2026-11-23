import sys
sys.stdout.reconfigure(encoding='utf-8')

with open('Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

import re
matches = [m.start() for m in re.finditer(r'<svg', text)]
print("SVG matches at:", matches)
for idx in matches:
    print("--- SVG FOUND ---")
    end_idx = text.find('</svg>', idx)
    print(text[idx:end_idx+6])
