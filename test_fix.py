import re
import html5lib

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

start = text.find('<svg viewBox="0 0 940 480"')
end = text.find('</svg>', start) + 6
svg = text[start:end]

# Replace HTML tags with SVG tags
fixed_svg = svg.replace('<b>', '<tspan font-weight="bold">').replace('</b>', '</tspan>')
fixed_svg = fixed_svg.replace('&bull;', '•').replace('&rarr;', '→')

# Wrap in minimal HTML to parse with html5lib
test_html = f"<!DOCTYPE html><html><body>{fixed_svg}</body></html>"
doc = html5lib.parse(test_html)

svg_el = doc.find('.//{http://www.w3.org/2000/svg}svg')
g_children = svg_el.findall('{http://www.w3.org/2000/svg}g')
print(f"Number of <g> children directly inside <svg> in SVG namespace: {len(g_children)}")
for i, g in enumerate(g_children):
    header = g.find('{http://www.w3.org/2000/svg}text')
    print(f"  g[{i+1}]: {header.text if header is not None else 'No text'}")
