from PIL import Image
for i in range(10):
    jpg = str(i) + ".jpg"
    pgm = str(i) + ".pgm"
    im = Image.open(jpg)
    im = im.convert('L')
    im.save(pgm)
