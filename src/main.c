#include <zephyr/ztest.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(lab5, LOG_LEVEL_DBG);

ZTEST_SUITE(lab5_tests, NULL, NULL, NULL, NULL, NULL);

//
// Test Message Queue functionality
//

struct msgq_message
{
  uint32_t counter;
};

K_MSGQ_DEFINE(test_msgq, sizeof(struct msgq_message), 10, 1);

// +++++ TAREA: DEFINE LOS DOS VALORES PARA QUE EL TEST PASE
const int num_slots_libres_en_cola = 0;
const int valor_contador_en_mensaje_recibido = 0;
// +++++

ZTEST(lab5_tests, test_1_msgq)
{
  struct msgq_message tx_data;
  tx_data.counter = 1;
  zassert_equal(k_msgq_put(&test_msgq, &tx_data, K_NO_WAIT), 0, "Failed to put message in queue");

  zassert_equal(k_msgq_num_free_get(&test_msgq), num_slots_libres_en_cola);

  struct msgq_message rx_data;
  zassert_equal(k_msgq_get(&test_msgq, &rx_data, K_FOREVER), 0);
  zassert_equal(rx_data.counter, valor_contador_en_mensaje_recibido);
}

//
// Test FIFO functionality
//

struct fifo_data_item
{
  void *fifo_reserved; /* 1st word reserved for use by FIFO */
  uint32_t counter;
};

K_FIFO_DEFINE(test_fifo);

ZTEST(lab5_tests, test_2_fifo)
{
  struct fifo_data_item tx_data = {NULL, 0};
  // +++++ TAREA: DEFINE EL VALOR DEL PUNTERO AL MENSAJE RECIBIDO PARA QUE EL TEST PASE
  const struct fifo_data_item *valor_puntero_rx_data1 = &tx_data;
  const struct fifo_data_item *valor_puntero_rx_data2 = &tx_data;
  // +++++

  tx_data.counter = 1;
  k_fifo_put(&test_fifo, &tx_data);

  struct fifo_data_item *rx_data = NULL;

  rx_data = k_fifo_get(&test_fifo, K_FOREVER);
  zassert_equal(rx_data->counter, 1);

  rx_data = k_fifo_get(&test_fifo, K_MSEC(0));
  zassert_equal(rx_data, valor_puntero_rx_data1);

  rx_data = k_fifo_get(&test_fifo, K_MSEC(100));
  zassert_equal(rx_data, valor_puntero_rx_data2);
}

//
// Test Mutex functionality
//

int counter_m = 0;
K_MUTEX_DEFINE(test_mutex);

void mutex_block_inc_counter(void *p1, void *p2, void *p3)
{
  while (1) {
    k_mutex_lock(&test_mutex, K_FOREVER);
    counter_m++;
    k_mutex_unlock(&test_mutex);

    k_msleep(10);
  }
}

K_THREAD_DEFINE(inc_counter_m_tid, 512, mutex_block_inc_counter, NULL, NULL, NULL, 5, 0, 0);

ZTEST(lab5_tests, test_3_mutex)
{
  k_mutex_lock(&test_mutex, K_FOREVER);
  counter_m = 100;
  k_mutex_unlock(&test_mutex);

  k_msleep(20);  // Dar tiempo a que el thread se ejecute

  k_mutex_lock(&test_mutex, K_FOREVER);
  // +++++ TAREA: SUSTITUYE EL ASSERT POR EL ADECUADO PARA QUE EL TEST PASE
  zassert_equal(counter_m, 100);
  // +++++
  k_mutex_unlock(&test_mutex);
}

//
// Test Semaphore functionality
//

int counter_s = 0;
K_SEM_DEFINE(test_sem, 0, 1);

void semaphore_block_inc_counter(void *p1, void *p2, void *p3)
{
  while (1) {
    if (!k_sem_take(&test_sem, K_FOREVER)) {
      counter_s++;
    }
  }
}

K_THREAD_DEFINE(inc_counter_s_tid, 512, semaphore_block_inc_counter, NULL, NULL, NULL, 5, 0, 0);

// +++++ TAREA: DEFINE EL VALOR FINAL DEL CONTADOR PARA QUE EL TEST PASE
const int valor_final_del_contador = 0;
// +++++

ZTEST(lab5_tests, test_4_semaphore)
{
  k_sleep(K_MSEC(10));  // Dar tiempo a que inicie el thread

// +++++ TAREA: HAZ LAS OPERACIONES NECESARIAS SOBRE EL SEMAFORO PARA QUE EL TEST PASE


// +++++

  k_sleep(K_MSEC(10));  // Dar tiempo suficiente para el thread haya hecho las operaciones

  zassert_equal(counter_s, 2);
  k_sem_reset(&test_sem);
  k_sleep(K_MSEC(200));   // Dar tiempo a que el thread siga ejecutandose
  zassert_equal(counter_s, valor_final_del_contador);
}

#if !defined(CONFIG_BOARD_NATIVE_SIM)
//
// Test Peripheral with device tree configuration
//
// Board Device Tree: zephyr/boards/st/nucleo_f412zg/nucleo_f412zg.dts
//
#define DEV_OUT DT_GPIO_CTLR(DT_INST(0, test_gpio), out_gpios)
#define DEV_IN  DT_GPIO_CTLR(DT_INST(0, test_gpio), in_gpios)
#define PIN_OUT DT_GPIO_PIN(DT_INST(0, test_gpio), out_gpios)
#define PIN_IN  DT_GPIO_PIN(DT_INST(0, test_gpio), in_gpios)

ZTEST(lab5_tests, test_5_hardware)
{
  static const struct device *const dev_in = DEVICE_DT_GET(DEV_IN);
  static const struct device *const dev_out = DEVICE_DT_GET(DEV_OUT);

  zassert_equal(device_is_ready(dev_in), 1);
  zassert_equal(device_is_ready(dev_out), 1);

  gpio_pin_configure(dev_in, PIN_IN, GPIO_INPUT);           // PB8
  gpio_pin_configure(dev_out, PIN_OUT, GPIO_OUTPUT_ACTIVE); // PB9

  zassert_equal(gpio_pin_get(dev_in, PIN_IN), 1);
  gpio_pin_set(dev_out, PIN_OUT, 0);
  zassert_equal(gpio_pin_get(dev_in, PIN_IN), 0);
}
#endif // !defined(CONFIG_BOARD_NATIVE_SIM)
