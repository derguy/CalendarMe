const CALENDAR_ID = '<Your Calendar-Id>';

function getEventString(events) {
  var str = '';
  for (var ii = 0; ii < events.length; ii++) {
    var event=events[ii]; 
    var myStatus = event.getMyStatus(); 
    str += Utilities.formatDate(event.getStartTime(), 'Europe/Berlin', 'HH:mm') + ' ' + event.getTitle() +'\n';
  }
  if (str === '') {
    str = '- nichts vor -\n'
  }
  return str;
}

function getWeekday(weekday) {
  switch(weekday) {
      case 1: return "Montag";
      case 2: return "Dienstag";
      case 3: return "Mittwoch";
      case 4: return "Do.";
      case 5: return "Freitag";
      case 6: return "Samstag";
      case 0: return "Sonntag";
  }
}

function doGet(e) {
  var cal = CalendarApp.getCalendarById(CALENDAR_ID);
  if (cal == undefined) {
    return ContentService.createTextOutput('no access to calendar');
  }
 
  const now = new Date();
  var start = new Date(); start.setHours(0, 0, 0); // start at midnight
  const oneday = 24*3600000; // [msec]
  
  var events = cal.getEvents(start, new Date(start.getTime() + oneday));
  var today = getEventString(events);
  
  var events = cal.getEvents(new Date(start.getTime() + oneday), new Date(start.getTime() + 2 * oneday));
  var tomorrow = getEventString(events);
  
  var data = {
    header1: "Heute: " + getWeekday(now.getDay()),
    body1: today, 
    header2: "Morgen", 
    body2: tomorrow
  }
  
  var clientId = e.parameter.clientId;
  Logger.log("clientId: " + clientId);
  
  var isNewData = true;
  if (clientId) {
    var properties = PropertiesService.getScriptProperties();
    if (properties) {
      var lastData = properties.getProperty('LAST_RESPONSE_' + clientId);
      Logger.log("lastData: " + lastData);
      Logger.log("data:" + data);
      
      if (lastData === JSON.stringify(data)) {
        isNewData = false;
      } else {
        properties.setProperty('LAST_RESPONSE_' + clientId, JSON.stringify(data));
      }
    } else {
      properties.setProperty('LAST_RESPONSE_' + clientId, JSON.stringify(data));
    }
  }
  
  var responseBody = JSON.stringify(Object.assign({}, data, { isNewData }));
  var response = ContentService.createTextOutput(responseBody).setMimeType(ContentService.MimeType.JSON);
                                                             
  Logger.log("response: " + responseBody);
  Logger.log("isNewData: " + isNewData);
  
  return response;
}