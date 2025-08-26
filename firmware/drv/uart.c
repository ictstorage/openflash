#include <global.h>
#include <register.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define CONFIG_UART_TX_QUENE_SIZE  (4*1024)
#define CONFIG_UART_TX_BUF_SIZE    256
#define CONFIG_UART_RX_BUF_SIZE    256
#define CONFIG_UART_FIFO_LENGTH    16
#define CONFIG_UART_TX_ENABLE_QUENE


typedef struct
{
  uint16_t size;
  uint16_t recv_count;
  uint16_t head;
  uint16_t tail;
  char recv_data[1];
}uart_quene;


typedef struct
{
  uint16_t size;
  uint16_t recv_count;
  uint16_t head;
  uint16_t tail;
  char recv_data[CONFIG_UART_RX_BUF_SIZE];
} rx_buffer;

typedef struct {
  uint16_t size;
  uint16_t recv_count;
  uint16_t head;
  uint16_t tail;
  char recv_data[CONFIG_UART_TX_QUENE_SIZE];
}tx_buffer;

static byte hex_to_ascii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static rx_buffer rb;
static tx_buffer tb;

void quene_init(uart_quene* quene,uint16_t size){
  quene->size = size;
  quene->recv_count = 0;
  quene->head = 0;
  quene->tail = 0;
}

void quene_push(uart_quene* quene, char data){
  if((quene->tail + 1)%quene->size == quene->head) return;
  quene->recv_data[quene->tail] = data;
  quene->tail = (quene->tail + 1) % quene->size;
  quene->recv_count++;
}

char quene_pop(uart_quene* quene){
  char data;
  if(quene->head == quene->tail) return 0;
  data = quene->recv_data[quene->head];
  quene->head = (quene->head + 1) % quene->size;
  quene->recv_count--;
  return data;
}

uint16_t quene_get_count(uart_quene* quene){
  return quene->recv_count;
}

void uart_buffer_init(){
  quene_init((uart_quene*)&rb,CONFIG_UART_RX_BUF_SIZE);
  quene_init((uart_quene*)&tb,CONFIG_UART_TX_QUENE_SIZE);
}

void fv_uart_init(uint32_t baudrate)
{
    uint32_t sys_clk = 125000000;
    uint16_t uart_divider = 0;

    while((rb_usr & USR_BUSY) == USR_BUSY);

    uart_divider = sys_clk / 16 /baudrate;
    rb_lcr = LCR_DL;
    rb_dll = uart_divider & 0xFF;
    rb_dlh = (uart_divider >> 8) & 0xFF;
    rb_lcr = LCR_SB1 | LCR_PN | LCR_WL8;

    rb_mcr = 0;
    rb_fcr = 0x07 | (2<<6) | (3<<4);
//    rb_ier = SET_BIT(0) | SET_BIT(2);

    uart_buffer_init();
}


void uart_put(int8_t c)
{
	rb_thr = c;
}

void uart_putc(char c)
{
#ifdef CONFIG_UART_TX_ENABLE_QUENE
	quene_push((uart_quene*)&tb,c);
#else
	while((rb_lsr & LSR_TEMT) == 0);
	uart_put(c);
#endif
}

void uart_puts(const char *buf)
{
  int32_t i,data_length = strlen(buf);

#ifdef CONFIG_UART_TX_ENABLE_QUENE
    for(i=0;i<data_length;i++){
      quene_push((uart_quene*)&tb,buf[i]);
    }
#else
  int32_t count;
  while(data_length >= 0 && *buf != 0){
	  while((rb_lsr & LSR_TEMT) == 0);
	  count = data_length <= CONFIG_UART_FIFO_LENGTH? data_length : CONFIG_UART_FIFO_LENGTH;
	  for(i=0;i<count;i++){
		  uart_put(*buf++);
	  }
	  data_length -= count;
  }
#endif
}

void uart_puts_right_now(char *buf){
	int32_t i,data_length = strlen(buf);
	int32_t count;
	while(data_length >= 0 && *buf != 0){
		while((rb_lsr & LSR_TEMT) == 0);
		count = data_length <= CONFIG_UART_FIFO_LENGTH? data_length : CONFIG_UART_FIFO_LENGTH;
		for(i=0;i<count;i++){
			uart_put(*buf++);
		}
		data_length -= count;
	}
}

void put_hex(uint8_t b_hex)
{
  uart_putc(hex_to_ascii[(b_hex >> 4) & 0x0f]);
  uart_putc(hex_to_ascii[ b_hex       & 0x0f]);
}

void put_2hex(uint16_t w_hex)
{
  put_hex((byte) ((w_hex >> 8) & 0x00ff));
  put_hex((byte) ( w_hex       & 0x00ff));
}

void put_4hex(uint32_t dw_hex)
{
  put_2hex((word) ((dw_hex >> 16) & 0x0000ffff));
  put_2hex((word) ( dw_hex        & 0x0000ffff));
}

void mem_dump(uint32_t addr, uint32_t count)
{
  uint8_t bValue;
  uint32_t dwCount;
  for (dwCount = 0; dwCount < count; dwCount++)
  {
    bValue = (*(uint8_t *)(addr + dwCount));
    if (dwCount % 16 == 0)
    {
      put_4hex(addr + dwCount);
      uart_puts("h: ");
    }
    put_hex(bValue);
    uart_puts(" ");
    if (dwCount != 0 && dwCount % 16 == 15)
      uart_putc('\r');
  }
}

void mem_dump_right_now(uint32_t addr, uint32_t count,uint8_t right_now)
{
  uint32_t i;
  uint32_t* p = (uint32_t*)addr;
  if (right_now){
	for(i=0;i<count;i++){		
		if(i % 16 == 0){
			fv_uart_print("\r\n0x%08X : ",p);
		}
		fv_uart_print("%08x  ",*p++);
	}
	uart_puts_right_now("\r\n");
  }
  else{
	for(i=0;i<count;i++){
		if( i % 16 == 0){
			fv_uart_print("\r\n0x%08X : ",p);
		}
		fv_uart_print("%08x  ",*p++);
	}
	uart_puts("\r\n");
  }
}

uint16_t get_uart_buffer(char* buf,uint32_t buflen){
  uint32_t i;
  for(i = 0; i < buflen && rb.recv_count>0; i++){
    buf[i] = quene_pop((uart_quene*)&rb);
  }
  return i;
}

void uart_read_data(void){
  char temp;
  while((rb_lsr & LSR_DR) == LSR_DR){
    temp = rb_rbr;
    quene_push((uart_quene*)&rb,temp);
  }
}

uint8_t uart_getc(void){
  uint8_t temp = 0;
  if((rb_lsr & LSR_DR) == LSR_DR){
    temp = rb_rbr;
  }
  return temp;
}

void uart_irq_handler(void){
  uint8_t temp = temp;
  switch(rb_iir & 0x0f){
    case 0x06:
      while(rb_rfl){
        temp = rb_rbr;
      }
    break;
    case 0x04:
      uart_read_data();
    break;
    case 0x0C:
      uart_read_data();
    break;
    default:break;
  }
  rb_lsr |= rb_lsr;
}

void uart_print_from_fifo(){
#ifdef CONFIG_UART_TX_ENABLE_QUENE
  int32_t i,count,data_length = quene_get_count((uart_quene*)&tb);
  uint8_t buf;
  while(data_length > 0){
	  while((rb_lsr & LSR_TEMT) == 0);
	  count = data_length <= CONFIG_UART_FIFO_LENGTH? data_length : CONFIG_UART_FIFO_LENGTH;
	  for(i=0;i<count;i++){
      buf = quene_pop((uart_quene*)&tb);
		  uart_put(buf);
	  }
	  data_length -= count;
  }
#endif
}

void fv_uart_debug(const char *fmt, ...)
{
#if defined(UART_DBG)
    char buffer[CONFIG_UART_TX_BUF_SIZE];
    va_list argptr;

    memset(buffer, 0, CONFIG_UART_TX_BUF_SIZE);
    va_start(argptr, fmt);
    vsnprintf(buffer, CONFIG_UART_TX_BUF_SIZE, fmt, argptr);
    va_end(argptr);
    uart_puts_right_now(buffer);
    //uart_puts(buffer);
#endif
}

void fv_uart_print(const char *fmt, ...)
{
    char buffer[CONFIG_UART_TX_BUF_SIZE];
    va_list argptr;

    memset(buffer, 0, CONFIG_UART_TX_BUF_SIZE);
    va_start(argptr, fmt);
    vsnprintf(buffer, CONFIG_UART_TX_BUF_SIZE, fmt, argptr);
    va_end(argptr);
    uart_puts_right_now(buffer);
    //uart_puts(buffer);
}

void fv_dbg_loop(byte b_flag)
{
    rb_cdbg_flag = b_flag;
    while(rb_cdbg_flag);
}

void fv_uart_chk(void)
{
    return;
}

