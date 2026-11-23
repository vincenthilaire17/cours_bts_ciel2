import html5lib

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    html = f.read()

doc = html5lib.parse(html)
# Find svg in doc
svgs = doc.findall('.//{http://www.w3.org/2000/svg}svg')
print(f"Number of SVG elements in SVG namespace: {len(svgs)}")
for i, s in enumerate(svgs):
    children = list(s)
    print(f"SVG {i+1} has {len(children)} direct children in SVG namespace")
    for c in children:
        print("  Child tag:", c.tag)
