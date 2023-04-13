
newFile = open("input.txt", "wb")

sectionText = bytearray(b'\x93\x01\x81\xc1\x33\x00\x00\x80')
sectionData = bytearray()

beq = bytearray(b'\x63\x84\x20\x00')
bne = bytearray(b'\x63\x94\x20\x00')
ld  = bytearray(b'\x83\xb0\x80\x00')
sb  = bytearray(b'\xa3\x8d\x10\x00')

# sectionText += bne
# sectionText += ld
sectionText += sb

newFile.write( (8).to_bytes(4, byteorder="little")) # entry point
newFile.write((len(sectionText) + 8).to_bytes(4, byteorder="little")) # data offset
# newFile.write(len(sectionData).to_bytes(8, byteorder="little"))
newFile.write( sectionText)