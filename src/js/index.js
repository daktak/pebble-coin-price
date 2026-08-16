/*jslint sub: true*/

function CoinPrice() {
  this.onResult = null;
}

// Coin identifiers (matches MinimalAnalog ticker configuration)
var CP_COIN_BTC = 1;
var CP_COIN_ETH = 2;
var CP_COIN_XRP = 3;
var CP_COIN_LTC = 4;
var CP_COIN_BCH = 5;
var CP_COIN_ETC = 6;

// Fiat currency identifiers (matches MinimalAnalog ticker configuration)
var CP_CURRENCY_USD = 1;
var CP_CURRENCY_AUD = 2;
var CP_CURRENCY_CAN = 3;
var CP_CURRENCY_NZD = 4;
var CP_CURRENCY_EUR = 5;
var CP_CURRENCY_PND = 6;

function parseCurrency(currency) {
  var currencyS = "usd";
  switch (currency) {
    case CP_CURRENCY_AUD:
      currencyS = "aud";
      break;
    case CP_CURRENCY_CAN:
      currencyS = "can";
      break;
    case CP_CURRENCY_NZD:
      currencyS = "nzd";
      break;
    case CP_CURRENCY_EUR:
      currencyS = "eur";
      break;
    case CP_CURRENCY_PND:
      currencyS = "pnd";
      break;
    default:
      currencyS = "usd";
  }
  return currencyS;
}

function parseCoin(coin) {
  var coinS = "BTC";
  switch (coin) {
    case CP_COIN_ETH:
      coinS = "ETH";
      break;
    case CP_COIN_XRP:
      coinS = "XRP";
      break;
    case CP_COIN_LTC:
      coinS = "LTC";
      break;
    case CP_COIN_BCH:
      coinS = "BCH";
      break;
    case CP_COIN_ETC:
      coinS = "ETC";
      break;
    default:
      coinS = "BTC";
  }
  return coinS;
}

function getRepString(rep) {
  if (rep < 2) {
    return rep.toFixed(2);
  }
  if (rep < 1000) {
    return rep.toFixed(0);
  }
  if (rep < 10000) {
    var rs = String(rep);
    return rs.charAt(0) + ',' + rs.substring(1);
  }
  return (rep / 1000).toFixed(rep % 1000 != 0) + 'K';
}

function queryCoin(messageId, coin, currency, api) {
  if (!api) {
    console.log("No CoinMarketCap API key configured, skipping ticker");
    return;
  }
  console.log("CMC api: " + api);
  var currencyS = parseCurrency(currency);
  var url = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?symbol=" + parseCoin(coin) + "&convert=" + currencyS.toUpperCase();
  console.log("requesting " + url);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", url);
  xhr.setRequestHeader("X-CMC_PRO_API_KEY", api);
  xhr.onload = function () {
    console.log(xhr.responseText);
    var responseJson = JSON.parse(xhr.responseText);
    var price = responseJson["data"][parseCoin(coin)]["quote"][currencyS.toUpperCase()]["price"];
    price = getRepString(price);
    console.log("Ticker:  " + price);
    Pebble.sendAppMessage({
      "CP_RESULT": price,
      "CP_ID": messageId
    });
  };
  xhr.send();
}

CoinPrice.prototype.appMessageHandler = function (e) {
  var payload = e.payload;
  var coin = payload["CP_COIN"];
  if (typeof coin === "undefined") {
    return; // not a request for us
  }
  var currency = typeof payload["CP_CURRENCY"] !== "undefined" ? payload["CP_CURRENCY"] : CP_CURRENCY_USD;
  var api = typeof payload["CP_API_KEY"] !== "undefined" ? payload["CP_API_KEY"] : "";
  var id = typeof payload["CP_ID"] !== "undefined" ? payload["CP_ID"] : 0;
  queryCoin(id, coin, currency, api);
};

module.exports = CoinPrice;
