
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Moto Maker"
  },

  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Dial colors"
      },

      {
      "type": "color",
      "messageKey": "colorBackground",
      "defaultValue": "000000",
      "label": "Background",
      "allowGray": "true",
      },

      {
        "type": "color",
        "messageKey": "colorHourHand",
        "defaultValue": "ffffff",
        "label": "Hour hand",
      },

      {
        "type": "color",
        "messageKey": "colorMinuteHand",
        "defaultValue": "ffffff",
        "label": "Minute hand",
      },

      {
        "type": "color",
        "messageKey": "colorSecondHand",
        "defaultValue": "ff0000",
        "label": "Second hand",
      },

      {
        "type": "color",
        "messageKey": "colorDot",
        "defaultValue": "ffffff",
        "label": "Dot",
        "description": "In the center of the watch face",
        "allowGray": "true",
      },

      {
        "type": "color",
        "messageKey": "colorHourMarkers",
        "defaultValue": "aaaaaa",
        "label": "Hour markers",
        "description": "12, 3, 6, and 9 o'clock markers",
      },

      {
        "type": "color",
        "messageKey": "colorMinuteMarkers",
        "defaultValue": "ffffff",
        "label": "Minute markers",
        "description": "Markers at every 5 minutes around the watch face",
      },

      {
        "type": "color",
        "messageKey": "logoColor",
        "defaultValue": "aaaaaa",
        "label": "Pebble logo",
      },
    ]
  },

  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Features"
      },

      {
      "type": "toggle",
      "messageKey": "enableSecondHand",
      "defaultValue": "true",
      "label": "Show second hand",
      "description": "Uses more battery"
      },

      {
        "type": "toggle",
        "messageKey": "enableVibrateOnDisconnect",
        "defaultValue": "true",
        "label": "Vibrate on disconnect",
        },
    ]
  },

  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
