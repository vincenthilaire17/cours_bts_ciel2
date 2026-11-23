with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

idx2 = text.find('<h2>2. Panorama')
idx3 = text.find('<h2>3. Matrice')
idx4 = text.find('<h2>4. Typologies')
idx5 = text.find('<h2>5. Guide')

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\sec2_4_dump.txt', 'w', encoding='utf-8') as out:
    out.write("=== SECTION 2 ===\n")
    out.write(text[idx2:idx3])
    out.write("\n=== SECTION 4 ===\n")
    out.write(text[idx4:idx5])

print("Dumped Section 2 and 4")
