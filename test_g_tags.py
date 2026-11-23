import html5lib

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    html = f.read()

doc = html5lib.parse(html)
# Find all elements whose tag is 'g' or ends with '}g'
all_g = [el for el in doc.iter() if el.tag.endswith('g')]
for g in all_g:
    print("Tag:", g.tag, "Parent:", doc.find(f".//*[@id='{g.get('id')}']"))
