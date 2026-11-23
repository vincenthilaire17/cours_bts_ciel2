import sys
import xml.etree.ElementTree as ET

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

start = text.find('<svg')
end = text.find('</svg>', start) + 6
svg_content = text[start:end]

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'w', encoding='utf-8') as f:
    f.write(svg_content)

print(f"SVG length: {len(svg_content)}")
try:
    tree = ET.fromstring(svg_content)
    print("XML Parsing SUCCESS! Root tag:", tree.tag)
except Exception as e:
    print("XML Parsing ERROR:", e)
