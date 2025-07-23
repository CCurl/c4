Encode and decode TEXT in a bitmap (*.BMP) file

Usage: to encode:  z" bmp-file" ->bmp  z" txt-file" ->txt   encode
       to decode:  z" bmp-file" ->bmp  z" txt-file" ->file  decode  ->stdout

64 var txt-fn  cell var txt-sz max-txt  40 1024 * ; max-txt var txt
64 var bmp-fn  cell var bmp-sz max-bmp1024 1024 * ; max-bmp var bmp

NOTES: t is txt addr, a is the txt byte, b is the next bitmap byte
->txt(fn--)txt-fn s-scpy drop ;
->bmp(fn--)bmp-fn s-scpy drop ;
read-file(addr max fn--sz)fopen-rb ?dupif02drop 0 exitthen
    >a  a@ fread  a> fclose ;
write-file(addr sz fn--)fopen-wb ?dupif02drop exitthen
    >a  a@ fwrite drop  a> fclose ;
skip-hdr(bmp--addr)dup $0a + d@ + ;
read-txt(--)txt max-txt txt-fn read-file txt-sz ! ;
read-bmp(--)bmp max-bmp bmp-fn read-file bmp-sz ! ;
write-txt(--)txt txt-sz @ txt-fn write-file;
write-bmp(--)bmp bmp-sz @ bmp-fn write-file ;
encode-1(--)a@ 2 /mod a!  c@b $FE and + c!b+ ;
encode-8(b--)a! 8 for encode-1 next ;
do-encode(--)bmp skip-hdr b!  txt t!  txt-sz @ for c@t+ encode-8 next  0 encode-8 ;
decode-1(--)8 for c@b+ 1 and >t next  0 a! 8 for a@ 2* t> + a! next ;
decode-all(--)decode-1 a@ifa@ emit decode-all exitthen;
do-decode(--)bmp skip-hdr b! decode-all ;
.files(--)txt-sz @ txt-fn bmp-sz @ bmp-fn ." bmp: %s (%d)%ntxt: %s (%d)%n" ;
init(--)txt max-txt 0 fill  bmp max-bmp 0 fill ;
encode(--)init read-txt read-bmp .files do-encode write-bmp ;
decode(--)init read-bmp do-decode;


