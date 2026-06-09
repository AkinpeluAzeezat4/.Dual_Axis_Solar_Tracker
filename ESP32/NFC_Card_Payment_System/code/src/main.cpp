#include <Arduino.h>

#include "Pins.h"
#include "button/button.h"
#include "NFC_module/NFC_module.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "WiFi/WiFi.h"
#include "oled_screen/oled_screen.h"
#include "led_indicator/led_indicator.h"
#include "sd_card/sd_card.h"
#include "p_data/p_data.h"
#include "payment/payment.h"
#include "transaction_log/transaction_log.h"
#include "error_handling/error_handling.h"
#include "rtc/rtc.h"
#include "reset/reset.h"

namespace
{
  enum AppState
  {
    APP_BOOT,
    APP_HOME,
    APP_WAIT_FOR_CARD,
    APP_RESULT_SUCCESS,
    APP_RESULT_FAILURE,
    APP_REGISTER_REQUIRED,
    APP_CARD_REGISTERED,
    APP_ERROR
  };

  AppState currentState = APP_BOOT;
  unsigned long stateSince = 0;
  String lastUID;
  String lastInfoText;
  String uiAmountText = "100";

  void setState(AppState nextState)
  {
    currentState = nextState;
    stateSince = millis();
  }

  void refreshFaults()
  {
    error_handling::setSdError(!sd_card::isReady());
    error_handling::setNfcError(!NFC_module::isReady());
    error_handling::setRtcError(!rtc::isReady());
  }

  void handleHomeButtons()
  {
    if (button::wasPressed(button::UP) || button::wasPressed(button::RIGHT))
    {
      payment::nextAmount();
      buzzer::beep(35);
    }

    if (button::wasPressed(button::DOWN) || button::wasPressed(button::LEFT))
    {
      payment::previousAmount();
      buzzer::beep(35);
    }

    if (button::wasPressed(button::OK))
    {
      setState(APP_WAIT_FOR_CARD);
      buzzer::beep(50);
    }
  }

  void handleWaitForCardButtons()
  {
    if (button::wasPressed(button::LEFT))
    {
      setState(APP_HOME);
      buzzer::beep(40);
    }
  }

  void updateStateMachine()
  {
    refreshFaults();

    if (error_handling::hasError())
    {
      lastInfoText = error_handling::getFirstErrorText();

      if (lastInfoText.isEmpty())
      {
        lastInfoText = "Module Fault";
      }

      if (currentState != APP_ERROR)
      {
        setState(APP_ERROR);
      }
    }

    switch (currentState)
    {
    case APP_BOOT:
      if (millis() - stateSince >= 2400)
      {
        setState(APP_HOME);
      }
      break;

    case APP_HOME:
      handleHomeButtons();
      break;

    case APP_WAIT_FOR_CARD:
    {
      handleWaitForCardButtons();

      String uid;
      if (NFC_module::consumeUID(uid))
      {
        lastUID = uid;
        payment::Result result = payment::processCard(uid);

        if (result.success)
        {
          lastInfoText = result.holderName;
          buzzer::playSuccess();
          setState(APP_RESULT_SUCCESS);
        }
        else if (result.reason == payment::PAY_UNKNOWN_CARD)
        {
          p_data::setPendingUID(uid);
          lastInfoText = uid;
          buzzer::playRegister();
          setState(APP_REGISTER_REQUIRED);
        }
        else
        {
          lastInfoText = result.message;
          buzzer::playFailure();
          setState(APP_RESULT_FAILURE);
        }
      }
    }
    break;

    case APP_RESULT_SUCCESS:
      if (millis() - stateSince >= 1800)
      {
        setState(APP_HOME);
      }
      break;

    case APP_RESULT_FAILURE:
      if (millis() - stateSince >= 2200)
      {
        setState(APP_HOME);
      }
      break;

    case APP_REGISTER_REQUIRED:
      if (!p_data::hasPendingUID())
      {
        buzzer::playSuccess();
        setState(APP_CARD_REGISTERED);
      }
      else if (button::wasPressed(button::LEFT))
      {
        setState(APP_HOME);
      }
      break;

    case APP_CARD_REGISTERED:
      if (millis() - stateSince >= 1500)
      {
        setState(APP_HOME);
      }
      break;

    case APP_ERROR:
      if (!error_handling::hasError())
      {
        setState(APP_HOME);
      }
      break;
    }

    uiAmountText = payment::getSelectedAmountText();
  }

  void renderScreen()
  {
    switch (currentState)
    {
    case APP_BOOT:
      oled_screen::update("welcome");
      break;

    case APP_HOME:
      oled_screen::update("main", uiAmountText.c_str());
      break;

    case APP_WAIT_FOR_CARD:
      oled_screen::update("pay", uiAmountText.c_str());
      break;

    case APP_RESULT_SUCCESS:
      oled_screen::update("payment_successful", uiAmountText.c_str());
      break;

    case APP_RESULT_FAILURE:
      oled_screen::update("payment_failure", lastInfoText.c_str());
      break;

    case APP_REGISTER_REQUIRED:
      oled_screen::update("register_needed", lastUID.c_str());
      break;

    case APP_CARD_REGISTERED:
      oled_screen::update("card_registered");
      break;

    case APP_ERROR:
      oled_screen::update("error", lastInfoText.c_str());
      break;
    }
  }

  void updateStatusLed()
  {
    if (error_handling::hasError())
    {
      led_indicator::update("fault");
    }
    else if (currentState == APP_WAIT_FOR_CARD)
    {
      led_indicator::update("active");
    }
    else
    {
      led_indicator::update("normal");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(300);

  Pins::begin();

  button::begin(Pins::BTN_UP_PIN, Pins::BTN_DOWN_PIN, Pins::BTN_LEFT_PIN, Pins::BTN_RIGHT_PIN, Pins::BTN_OK_PIN, true);
  buzzer::begin(Pins::BUZZER_PIN);
  // battery_level::begin(Pins::BATTERY_ADC_PIN, 1.61f);
  battery_level::begin();

  oled_screen::begin(Pins::I2C_SDA_PIN, Pins::I2C_SCL_PIN);
  led_indicator::begin(Pins::LED_PIN);

  rtc::begin();
  sd_card::begin(Pins::SD_CS_PIN, Pins::SPI_MOSI_PIN, Pins::SPI_MISO_PIN, Pins::SPI_SCK_PIN);
  p_data::begin();
  transaction_log::begin();
  payment::begin();

  NFC_module::begin(Pins::NFC_SS_PIN, Pins::NFC_RST_PIN, Pins::SPI_MOSI_PIN, Pins::SPI_MISO_PIN, Pins::SPI_SCK_PIN);

  wifi_service::begin();

  error_handling::begin();
  reset::begin();

  refreshFaults();
  setState(APP_BOOT);
}

void loop()
{
  button::update();
  buzzer::update();
  battery_level::update();
  error_handling::setBatteryError(battery_level::isLow());

  if (battery_level::shouldSleep())
  {
    oled_screen::update("error", "Low Battery");
    delay(1500);
    battery_level::sleepNow();
  }
  rtc::update();
  NFC_module::update();
  wifi_service::update();
  reset::update();
  updateStateMachine();
  updateStatusLed();
  renderScreen();
}
