---
layout: page
title: Schedule
subtitle: Check out our practice schedule this fall
permalink: /schedule/
custom_js:
 - format-google-calendar.min
 - jquery-2.1.1
---
## Practice Schedule
Schedule may be subject to change, look to the weekly email for the latest, which comes out on Sunday evenings.

To remain an active member, come to 2 practices a week!

|    |             Mon            |           Tues           |      Wed      |           Thurs          |     Fri     |
|:--:|:--------------------------:|:------------------------:|:-------------:|:------------------------:|:-----------:|
| AM |         6a Swim SAC        |                          | 6a Spin TRECS |                          | 6a Swim SAC |
| PM | Run at Bearden Beer Market | Run / Bike / Activity 5:30p |               | Run / Bike / Activity 5:30p |             |

<ul id="events-upcoming">
</ul>
<ul id="events-past">
</ul>

<script>
formatGoogleCalendar.init({
        calendarUrl: 'https://www.googleapis.com/calendar/v3/calendars/7caamt6q92v1vbrna0ergpoqks%40group.calendar.google.com/events?timeMin=2015-06-03T10%3A00%3A00-04%3A00&key=AIzaSyC39nF_t-bAogVVXR9dLGLiSEDywUt7Fgc',
        past: true,
        upcoming: true,
        sameDayTimes: true,
        pastTopN: 1,
        upcomingTopN: -1,
        itemsTagName: 'li',
        upcomingSelector: '#events-upcoming',
        pastSelector: '#events-past',
        upcomingHeading: '<h2>Upcoming events</h2>',
        pastHeading: '<h2>Past events</h2>',
        format: ['*date*', ': ', '*summary*', ' <br/> ', '*description*', '<ol>', '*location*', '</ol>']
});
</script>
