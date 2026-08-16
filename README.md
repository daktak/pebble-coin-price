# pebble-coin-price

Pebble library to fetch a cryptocurrency price and return it to the watch.

The watch requests a coin, a fiat currency, and a CoinMarketCap API key; the
companion phone app (pkjs) queries the CoinMarketCap API, formats the price,
and sends the result back. The value is delivered to the watch via a C
callback.

Copied and adapted from the ticker functionality of
https://github.com/daktak/MinimalAnalog

## Usage

`pebble package install pebble-coin-price`

### C

```c
#include <pebble-coin-price/pebble-coin-price.h>

static void my_handler(const char *price, CoinPriceStatus status, int message_id) {
  if (status == CoinPriceStatusAvailable) {
    // price is the formatted price string, e.g. "60000" or "60K"
    APP_LOG(APP_LOG_LEVEL_INFO, "price: %s", price);
  }
}

static void init(void) {
  coin_price_init();
  coin_price_set_handler(my_handler);
  coin_price_fetch(CoinPriceCoinBTC, CoinPriceCurrencyUSD, "YOUR_CMC_API_KEY");
}

static void deinit(void) {
  coin_price_deinit();
}
```

### JS (in your host `pkjs`)

```js
var CoinPrice = require("pebble-coin-price");
var coinPrice = new CoinPrice();

Pebble.addEventListener("appmessage", function (e) {
  coinPrice.appMessageHandler(e);
});
```

## Message keys

The package declares the following message keys, which are merged into your app
automatically:

- `CP_COIN` — the coin to look up (see `CoinPriceCoin`).
- `CP_CURRENCY` — the fiat currency to convert to (see `CoinPriceCurrency`).
- `CP_API_KEY` — your CoinMarketCap Pro API key.
- `CP_RESULT` — the formatted price text returned by the phone.
- `CP_ID` — a correlation id echoed back with the result.
