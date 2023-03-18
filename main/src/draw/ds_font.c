

#include "ds_font.h"
#include "esp_partition.h"
#include <string.h>

typedef struct{
    uint16_t min;
    uint16_t max;
    uint8_t  bpp;
    uint8_t  reserved[3];
}x_header_t;

typedef struct{
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t  ofs_x;
    int8_t  ofs_y;
    uint8_t r;
}glyph_dsc_t;


#define FLASH_ADDR_DATA 0x40
#define FLASH_ADDR_NVS 0x01

static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0x9fa0,
    .bpp = 4,
};



static uint8_t __g_font_buf[300];//如bin文件存在SPI FLASH可使用此buff

const esp_partition_t* partition_res=NULL;

static uint8_t *user_font_getdata(int offset, int size)
{
    //如字模保存在SPI FLASH, SPIFLASH_Read(__g_font_buf,offset,size);
    //如字模已加载到SDRAM,直接返回偏移地址即可如:return (uint8_t*)(sdram_fontddr+offset);
    static uint8_t first_in = 1;  
    if(first_in==1)
    {
        //这个函数第一个参数是我们分区表的第四行的，第二列的参数，第二个是第三列的值      
        partition_res=esp_partition_find_first(FLASH_ADDR_DATA,FLASH_ADDR_NVS,NULL);
        first_in=0;
        if (partition_res == NULL)
        {
            printf("Failed to open file for reading\n");
            return NULL;
        }
        else
        {
             printf("Successfully open file for reading\n");
        }
    }
    esp_err_t res=esp_partition_read(partition_res,offset,__g_font_buf,size);//读取数据
    if(res!=ESP_OK)
    {
        printf("Failed to reading %x \n",offset);
    }
    return __g_font_buf;
}

static uint32_t user_font_get_glyph_dsc(uint32_t unicode_letter,glyph_dsc_t * gdsc) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return 0;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)user_font_getdata(unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        uint32_t pos = p_pos[0];
        glyph_dsc_t *mdsc = (glyph_dsc_t*)user_font_getdata(pos, sizeof(glyph_dsc_t));
        memcpy(gdsc,mdsc,sizeof(glyph_dsc_t));
        // printf("pos = %x\n",pos);
        // printf("gdsc->adv_w = %d\n",gdsc->adv_w);
        // printf("gdsc->box_h = %d\n",gdsc->box_h);
        // printf("gdsc->box_w = %d\n",gdsc->box_w);
        // printf("gdsc->ofs_x = %d\n",gdsc->ofs_x);
        // printf("gdsc->ofs_y = %d\n",gdsc->ofs_y);
        // printf("__g_xbf_hd.bpp = %d\n",__g_xbf_hd.bpp);
        return pos;
    }
    return 0;
}

static const uint8_t * user_font_get_bitmap(uint32_t pos,int size) {
    return user_font_getdata(pos+sizeof(glyph_dsc_t), size);    
}

int ds_get_bitmap(uint32_t letter,uint8_t *bitmap_buf,uint8_t *box_w,uint8_t *box_h,uint8_t *offset_x,uint8_t *offset_y){
    glyph_dsc_t gdsc;
    uint32_t pos = user_font_get_glyph_dsc(letter,&gdsc);
    if(pos != 0){
        int size = gdsc.box_w*gdsc.box_h*__g_xbf_hd.bpp/8;
        *box_w = gdsc.box_w;
        *box_h = gdsc.box_h;
        *offset_x = gdsc.ofs_x;
        *offset_y = gdsc.ofs_y;
        user_font_get_bitmap(pos,size);
        memcpy(bitmap_buf,__g_font_buf,size);
        return size;
    }
    return 0;
}


void test_ds_font(){
    uint8_t buf[300];
    uint8_t box_w,box_h = 0;
    uint8_t offset_x,offset_y = 0;
    int size = ds_get_bitmap(0x0041,buf,&box_w,&box_h,&offset_x,&offset_y);
    printf("box_w*h = %d*%d\n",box_w,box_h);
    for(int index = 0;index < size ;index ++){
        printf("%x ",buf[index]);
    }
}

//SimHei,,-1
//字模高度：18
//XBF字体,外部bin文件
// lv_font_t myFont = {
//     .get_glyph_bitmap = __user_font_get_bitmap,
//     .get_glyph_dsc = __user_font_get_glyph_dsc,
//     .line_height = 18,
//     .base_line = 0,
// };


// /**
//  * Decode an UTF-8 character from a string.
//  * @param txt pointer to '\0' terminated string
//  * @param i start byte index in 'txt' where to start.
//  *          After call it will point to the next UTF-8 char in 'txt'.
//  *          NULL to use txt[0] as index
//  * @return the decoded Unicode character or 0 on invalid UTF-8 code
//  */
// static uint32_t lv_txt_utf8_next(const char * txt, uint32_t * i)
// {
//     /* Unicode to UTF-8
//      * 00000000 00000000 00000000 0xxxxxxx -> 0xxxxxxx
//      * 00000000 00000000 00000yyy yyxxxxxx -> 110yyyyy 10xxxxxx
//      * 00000000 00000000 zzzzyyyy yyxxxxxx -> 1110zzzz 10yyyyyy 10xxxxxx
//      * 00000000 000wwwzz zzzzyyyy yyxxxxxx -> 11110www 10zzzzzz 10yyyyyy 10xxxxxx
//      * */

//     uint32_t result = 0;

//     /*Dummy 'i' pointer is required*/
//     uint32_t i_tmp = 0;
//     if(i == NULL) i = &i_tmp;

//     /*Normal ASCII*/
//     if((txt[*i] & 0x80) == 0) {
//         result = txt[*i];
//         (*i)++;
//     }
//     /*Real UTF-8 decode*/
//     else {
//         /*2 bytes UTF-8 code*/
//         if((txt[*i] & 0xE0) == 0xC0) {
//             result = (uint32_t)(txt[*i] & 0x1F) << 6;
//             (*i)++;
//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += (txt[*i] & 0x3F);
//             (*i)++;
//         }
//         /*3 bytes UTF-8 code*/
//         else if((txt[*i] & 0xF0) == 0xE0) {
//             result = (uint32_t)(txt[*i] & 0x0F) << 12;
//             (*i)++;

//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += (uint32_t)(txt[*i] & 0x3F) << 6;
//             (*i)++;

//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += (txt[*i] & 0x3F);
//             (*i)++;
//         }
//         /*4 bytes UTF-8 code*/
//         else if((txt[*i] & 0xF8) == 0xF0) {
//             result = (uint32_t)(txt[*i] & 0x07) << 18;
//             (*i)++;

//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += (uint32_t)(txt[*i] & 0x3F) << 12;
//             (*i)++;

//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += (uint32_t)(txt[*i] & 0x3F) << 6;
//             (*i)++;

//             if((txt[*i] & 0xC0) != 0x80) return 0; /*Invalid UTF-8 code*/
//             result += txt[*i] & 0x3F;
//             (*i)++;
//         }
//         else {
//             (*i)++; /*Not UTF-8 char. Go the next.*/
//         }
//     }
//     return result;
// }

// void txt_to_code(const char * txt){
//     uint32_t i = 0;
//     while(txt[i] != '\0') {
//         uint32_t data = lv_txt_utf8_next(txt, &i);
//         printf("data %x\n",data);
//     }
// }

