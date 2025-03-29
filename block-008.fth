(Encode TEXT file into BMP file)

64 var txt-fn  cell var txt-sz max-txt  20 1024 * ; max-txt var txt
64 var bmp-fn  cell var bmp-sz max-bmp1024 1024 * ; max-bmp var bmp

read-file(addr max fn--sz)fopen-rb ?dupif02drop 0 exitthen
    >a  a@ fread  a> fclose ;

write-file(addr sz fn--)fopen-wb ?dupif02drop exitthen
    >a  a@ fwrite drop  a> fclose ;

NOTES: t is txt addr, a is the txt byte, b is the next bitmap byte
skip-hdr(bmp--addr)dup $0a + d@ + ;
read-txt (--)txt max-txt txt-fn read-file txt-sz ! ;
read-bmp (--)bmp max-bmp bmp-fn read-file bmp-sz ! ;
write-txt(--)txt txt-sz @ txt-fn write-file;
write-bmp(--)bmp bmp-sz @ bmp-fn write-file ;
encode-1(--)a@ 2 /mod a!  @b $FE and + !b+ ;
encode-8(b--)a! 8 for encode-1 next ;
do-encode(--)bmp skip-hdr b!  txt t!  txt-sz @ for @t+ encode-8 next  0 encode-8 ;
do-decode(--)bmp skip-hdr b! ." -not impl yet-" ;
.files(--)txt-sz @ txt-fn bmp-sz @ bmp-fn ." %s: %d%n%s: %d%n" ;
init(--)txt max-txt 0 fill  bmp max-bmp 0 fill ;
encode(--)init read-txt read-bmp .files do-encode write-bmp ;
decode(--)init read-bmp do-decodewrite-txt;

txt-fn z" /home/chris/docs/account-manager.html" s-cpy
bmp-fn z" /home/chris/docs/ovi.bmp" s-cpy drop




