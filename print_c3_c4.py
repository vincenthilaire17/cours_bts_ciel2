with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    lines = f.readlines()

print("=== Card 3 ===")
for j in range(100, 145):
    print(lines[j], end='')

print("=== Card 4 ===")
for j in range(145, 173):
    print(lines[j], end='')
