#include <zephyr.h>
//#include <misc/printk.h>
#include <sys/printk.h>
//#include <uart.h>
#include <drivers/uart.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// #include <UartRingbuffer.h>




//static u8_t uart_buf[1024];
static K_FIFO_DEFINE(fifo_uart_tx_data);
static K_FIFO_DEFINE(fifo_uart_rx_data);
static uint8_t uart_buf[1024];

//static bool AT_OK;
//static bool AT_CWMODE_OK;
//static bool AT_CWLAP_OK;


//enum uart_config_parity UART_CFG_PARITY_NONE;
//enum uart_config_stop_bits UART_CFG_STOP_BITS_1;
//enum uart_config_data_bits UART_CFG_DATA_BITS_8;
//enum uart_config_flow_ctrl UART_CFG_FLOW_CTRL_NONE;

struct uart_data_t {
    void       *fifo_reserved;
    uint8_t    data[1024];
    uint16_t   len;
};

const struct uart_config uart_cfg = {
    .baudrate   = 4800,
    .parity     = UART_CFG_PARITY_NONE,
    .stop_bits  = UART_CFG_STOP_BITS_1,
    .data_bits  = UART_CFG_DATA_BITS_8,
    .flow_ctrl  = UART_CFG_FLOW_CTRL_NONE,
    };
/*
struct uart_config {
    uint32_t baudrate;
    uint8_t parity = UART_CFG_PARITY_NONE;
    uint8_t stop_bits = UART_CFG_STOP_BITS_1;
    uint8_t data_bits = UART_CFG_DATA_BITS_8;
    uint8_t flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
}; */

const struct device *uart_dev;







static void uart_irq_handler(const struct device *x, void *context)
{
   
   /*switch (case)
   {
   case 1: if (AT_OK)
    {
        uint8_t buf[] = {0x41, 0x54, 0x0d, 0x0a};
    }
    break;

   case 2: if(AT_CWMODE_OK)
    {
        uint8_t buf[] = {0x41, 0x54, 0x2b, 0x43 ,0x57, 0x4d, 0x4f, 0x44, 0x45, 0x3d, 0x33, 0x0d, 0x0a};
    }
    break;

   case 3: if (AT_CWLAP_OK)
   {
   uint8_t buf[] = {0x41, 0x54, 0x2b, 0x43, 0x57, 0x4c, 0x41, 0x50, 0x0d, 0x0a};
   }
   break;

   default: end 
   
   } */


    /*
    if (AT_OK)
    {
        uint8_t buf[] = {0x41, 0x54, 0x0d, 0x0a};
        printk("Sent data is: %s ", buf);
    }

    if(AT_CWMODE_OK)
    {
        uint8_t buf[] = {0x41, 0x54, 0x2b, 0x43 ,0x57, 0x4d, 0x4f, 0x44, 0x45, 0x3d, 0x33, 0x0d, 0x0a};
        printk("Sent data is: %s ", buf);
    }
    
    if (AT_CWLAP_OK)
    {
    
        uint8_t buf[] = {0x41, 0x54, 0x2b, 0x43, 0x57, 0x4c, 0x41, 0x50, 0x0d, 0x0a};
        printk("Sent data is: %s ", buf);
    }*/


    uint8_t buf[] = {0x41, 0x54, 0x2b, 0x43, 0x57, 0x4c, 0x41, 0x50, 0x0d, 0x0a};
    uint8_t succ = 0;
    if (uart_irq_tx_ready(x)) {
        printk("txReady");
        printk("Sent data is: %s ", buf);
        /*uint8_t data[] = {0x41, 0x54, 0x0d, 0x0a};

        for (int i = 0; i < sizeof(buf)/sizeof(buf[0]); ++i)
        {
            uart_poll_out(x, buf[i]);

        } */
        succ = uart_fifo_fill(x, buf, sizeof(buf));
        printk("sending success: %i", succ);
        // scanf("%s",buf);
    /*  uart_poll_out(x, buf[0]);
        uart_poll_out(x, buf[1]);
        uart_poll_out(x, buf[2]);
        uart_poll_out(x, buf[3]); */
        uart_irq_tx_disable(x);
    }

    if (uart_irq_rx_ready(x)) {
         uint8_t buf[100];
    //  buf.toCharArray((char *)buf, sizeof(buf));
        //int ssid = 1;
        //static uint8_t buf[sizeof(ssid)];
        //int i, j;
        
        
        //memcpy(buf, &ssid, size of(buf));

        int len = uart_fifo_read(x, buf, sizeof(buf));
        //strcat(buf,buf);
        // printk("Received data is: %.*c \r \n  ", buf);
          printk("Received data is: %.*s \r \n  ", len, buf);
        printk("len is : %d ", len);
        //printk("Received data in int is:%i\r\n", buf);

        /*
        char *str;
        str = (char*) malloc(1024);              
        int count = sizeof(buf); 
        char *buff = malloc(10 * count + 1); 
        for(int k = 0; k < len; k++)
        sprintf(buff + k * 10, "0x%02X ", buf[k] & 0xFF);
        buff[10 * len - 1] = 0;
        printk("<Received data is : [%.*s]\n",len, buf);*/
       // if (len) {
           // printk("read %d bytes\n", len);
      //  }
    }
}

void uart_cb(const struct device *x)
{
    uart_irq_update(x); // Start processing interrupts in ISR.
    int data_length = 0;

    if (uart_irq_rx_ready(x)) { // Check if UART RX buffer has a received char.
        data_length = uart_fifo_read(x, uart_buf, sizeof(uart_buf)); // Read data from FIFO.
        uart_buf[data_length] = 0;
                }
                printk("Received data is: %s \r \n ", uart_buf);

            if (uart_irq_tx_ready(x)) 
            {
                struct uart_data_t *buf =
                k_fifo_get(&fifo_uart_tx_data, K_NO_WAIT);
                uint16_t written = 0;

              /* Nothing in the FIFO, nothing to send */
        if (!buf) {
            uart_irq_tx_disable(x);
            return;
        }

                while (buf->len > written) {
            written += uart_fifo_fill(x,
                          &buf->data[written],
                          buf->len - written);
                          uart_tx(uart_dev, uart_buf, 5, 5000);
        }

                while (!uart_irq_tx_complete(x)) { // Wait for the last byte to get shifted out of the module
             
        }

        if (k_fifo_is_empty(&fifo_uart_tx_data)) {
            uart_irq_tx_disable(x);
        }

                k_free(buf);



}
}

void uart_sendCOM(const struct device *x,  uint8_t *Cont)
{
                 uint8_t succ = 1;
                 uint8_t succ2 = 1;
                 uint16_t len = strlen(Cont);
                 succ2 = uart_fifo_fill(x, Cont,len );
                 uart_irq_tx_enable(x);
                 succ = uart_tx(x, Cont, len, 100);
                 printk("uart_tx success: %i", succ);

}

static void interrupt_driven(const struct device *dev)
{
    //uint8_t c = 0xff;

    uart_irq_callback_user_data_set(dev, uart_irq_handler, NULL);
    uart_irq_rx_enable(dev);
    while (1) {
        uart_irq_tx_enable(dev);
        k_sleep(K_MSEC(5000));

        //uart_poll_out(dev, c);
        //k_sleep(K_MSEC(100));
    }
}

void main(void)
{


    uart_dev = device_get_binding("mainuart");
        if (!uart_dev) {
                    printk("error \r\n");
                }
                bool ret;
                ret = uart_configure(uart_dev, &uart_cfg);
                if (ret != 0) {
                    printk("UART can not be configured! \n");
                }
                while(1){
                    interrupt_driven(uart_dev);
                }


       /* unit32_t err_code;
       
       err_code = functions();
       APP_ERROR_CHECK(err_code);
       
       */


       /* uart_irq_rx_disable(uart_dev);
                uart_irq_tx_disable(uart_dev);
        uart_irq_callback_set(uart_dev, uart_irq_handler); // Set UART interrupt callback
        uart_irq_rx_enable(uart_dev);

        uart_irq_tx_enable(uart_dev);

        printk("Testing1: \n");
        //uart_sendCOM(uart_dev, "AT+GMR\r\n");
        //uart_fifo_fill(uart_dev, "AT\r\n" ,sizeof("AT\r\n"));

        k_sleep(K_MSEC(100));
                printk("UART loopback start!\n");

    while (1) {
        //do {
                printk("SendingAT..... \n");
                uart_sendCOM(uart_dev, "AT\r\n");
                k_sleep(K_MSEC(5000));
                //waiting for sending UDP Package
        //}while(k_sem_take(&k_sended, K_MSEC(5000)) !=0);
        //printk("recieved");*/

    }



/* unsigned char chIn = 'x';
int resIn = 0;

struct device *uart = device_get_binding("UART_1");

printk("Starting Wifi connection!\n");
while(1)
{
resIn = uart_poll_in(uart,&chIn);
printk("%d %c\r\n",resIn,chIn);
if(resIn == 0)
{
printk("chIn: %c",chIn);
uart_poll_out(uart,chIn);
}
k_sleep(1000);
}

printk("UART loopback start!\n");
while (1)
{
k_cpu_idle();
} */