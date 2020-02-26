 
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
        "defaultValue": "Colors"
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
        "allowGray": "true",
      },

      {
        "type": "color",
        "messageKey": "colorMinuteHand",
        "defaultValue": "ffffff",
        "label": "Minute hand",
        "allowGray": "true",
      },

      {
        "type": "color",
        "messageKey": "colorSecondHand",
        "defaultValue": "ff0000",
        "label": "Second hand",
        "allowGray": "true",
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
        "messageKey": "colorMarkers",
        "defaultValue": "aaaaaa",
        "label": "Markers",
        "description": "Hour markers, minute markers, and pebble logo",
        "allowGray": "true",
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
        "messageKey": "enableHideDotOnDisconnect",
        "defaultValue": "true",
        "label": "Hide dot when disconnected",
        "description": "Hides the dot from the middle of the watch face when not connected to your phone"
        },
    ]
  },
  
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];