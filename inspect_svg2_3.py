with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

import re
svg_matches = list(re.finditer(r'<svg[\s\S]*?</svg>', text))
print("SVG 2 title:")
s2 = svg_matches[1].group(0)
for t in re.findall(r'<text[^>]*>.*?</text>', s2)[:5]:
    print(" ", t)

print("SVG 3 title:")
s3 = svg_matches[2].group(0)
for t in re.findall(r'<text[^>]*>.*?</text>', s3)[:5]:
    print(" ", t)
