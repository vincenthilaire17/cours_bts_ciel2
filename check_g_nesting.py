with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    text = f.read()

import re
tags = re.findall(r'(</?g\b[^>]*>)', text)
level = 0
for t in tags:
    if t.startswith('</'):
        level -= 1
        print(f"{'  '*level}{t} (level {level})")
    else:
        print(f"{'  '*level}{t} (level {level})")
        level += 1
print(f"Final nesting level: {level}")
