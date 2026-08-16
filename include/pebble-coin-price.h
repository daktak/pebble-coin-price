#pragma once
#include <pebble.h>

typedef enum {
  CoinPriceStatusPending = 0,
  CoinPriceStatusAvailable,
  CoinPriceStatusFailed,
  CoinPriceStatusBluetoothDisconnected,
} CoinPriceStatus;

// Coin identifiers (matches MinimalAnalog ticker configuration)
typedef enum {
  CoinPriceCoinBTC = 1,
  CoinPriceCoinETH = 2,
  CoinPriceCoinXRP = 3,
  CoinPriceCoinLTC = 4,
  CoinPriceCoinBCH = 5,
  CoinPriceCoinETC = 6,
} CoinPriceCoin;

// Fiat currency identifiers (matches MinimalAnalog ticker configuration)
typedef enum {
  CoinPriceCurrencyUSD = 1,
  CoinPriceCurrencyAUD = 2,
  CoinPriceCurrencyCAN = 3,
  CoinPriceCurrencyNZD = 4,
  CoinPriceCurrencyEUR = 5,
  CoinPriceCurrencyPND = 6,
} CoinPriceCurrency;

typedef void (*CoinPriceHandler)(const char *price, CoinPriceStatus status, int message_id);

void coin_price_init(void);
void coin_price_deinit(void);
void coin_price_set_handler(CoinPriceHandler handler);
int coin_price_fetch(CoinPriceCoin coin, CoinPriceCurrency currency, const char *api_key);
