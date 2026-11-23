with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    text = f.read()

import re
groups = re.findall(r'<g transform=\"translate\((\d+),\s*(\d+)\)\">([\s\S]*?)</g>\s*(?=<g transform|</svg>)', text)
print(f"Total groups found: {len(groups)}")
for i, (x, y, content) in enumerate(groups):
    title_m = re.search(r'<text[^>]*font-weight=[\"\']bold[\"\'][^>]*>(.*?)</text>', content)
    title = title_m.group(1) if title_m else "No title"
    print(f"Group {i+1} at ({x}, {y}): {title}")
    # print text elements
    texts = re.findall(r'<text[^>]*>(.*?)</text>', content)
    print("  Texts:", texts)
