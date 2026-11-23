import re

with open(r'C:\Users\vince\OneDrive\Documents\GitHub\cours_bts_ciel2\Synthese_Bluetooth_WiFi_BTS_CIEL2.html', 'r', encoding='utf-8') as f:
    text = f.read()

print("Checking for '**':", [m.span() for m in re.finditer(r'\*\*', text)])
for m in re.finditer(r'\*\*', text):
    print("  Context:", repr(text[max(0, m.start()-40):min(len(text), m.end()+40)]))

print("Checking for '*...*':", [m.group(0) for m in re.finditer(r'\*[a-zA-Z0-9\s]+\*', text)])
print("Checking for '$':", [m.span() for m in re.finditer(r'\$', text)])
