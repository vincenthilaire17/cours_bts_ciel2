with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\svg1_extracted.svg', 'r', encoding='utf-8') as f:
    lines = f.readlines()

card_lines = [30, 66, 101, 146, 174, 218]
for idx in card_lines:
    print(f"=== Card at line {idx} ===")
    for j in range(idx - 1, min(len(lines), idx + 25)):
        print(f"{j+1}: {lines[j]}", end='')
