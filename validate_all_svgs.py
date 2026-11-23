import re
import xml.etree.ElementTree as ET

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

svgs = re.findall(r'<svg[\s\S]*?</svg>', text)
for i, s in enumerate(svgs):
    print(f"=== SVG {i+1} ===")
    # Check if replacing standard HTML entities allows strict XML parsing
    s_xml = (s.replace('&bull;', '•')
              .replace('&rarr;', '→')
              .replace('&times;', '×')
              .replace('&oplus;', '⊕')
              .replace('&Omega;', 'Ω')
              .replace('&approx;', '≈')
              .replace('&mu;', 'μ')
              .replace('&Delta;', 'Δ'))
    try:
        ET.fromstring(s_xml)
        print(f"SVG {i+1} XML valid (after entity replacement)!")
    except Exception as e:
        print(f"SVG {i+1} XML ERROR:", e)
