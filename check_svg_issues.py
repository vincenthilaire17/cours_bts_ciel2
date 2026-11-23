import re

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

svgs = re.findall(r'<svg[\s\S]*?</svg>', text)

for i, svg in enumerate(svgs):
    print(f"\n--- Checking SVG {i+1} ---")
    html_tags = re.findall(r'<(?:b|i|strong|em|span|div|p|br|hr)\b[^>]*>', svg)
    print(f"HTML tags inside SVG: {html_tags}")
    entities = re.findall(r'&[a-zA-Z]+;', svg)
    # in XML only &amp;, &lt;, &gt;, &quot;, &apos; are standard
    non_std = [e for e in entities if e not in ['&amp;', '&lt;', '&gt;', '&quot;', '&apos;']]
    print(f"Non-standard XML entities: {set(non_std)}")
