with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i in range(max(0, 50), min(len(lines), 75)):
    print(f"{i+1}: {repr(lines[i])}")
