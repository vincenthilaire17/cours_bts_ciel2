with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    text = f.read()

lines = text.splitlines()
print(f"Total lines: {len(lines)}")
for i, line in enumerate(lines):
    if any(tag in line for tag in ['<g', '</g', '<text', '</text', '<rect', '<circle', '<line', '<path']):
        if '<g transform' in line:
            print(f"Line {i+1}: {line}")
