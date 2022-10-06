/* * Data converted from /home/bot/code/esp-idf/files-2-flash/main/files/sample.txt
 */
.data
.section .rodata.embedded

.global sample_txt
sample_txt:

.global _binary_sample_txt_start
_binary_sample_txt_start: /* for objcopy compatibility */
.byte 0x61, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x65

.global _binary_sample_txt_end
_binary_sample_txt_end: /* for objcopy compatibility */


.global sample_txt_length
sample_txt_length:
.word 13
