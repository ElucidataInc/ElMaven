var Raven = require('raven');
Raven.config('https://5428a76c424142128a3ff1c04e5e342e:e7ae1bff365a46e9820e573523871f8e@sentry.io/294375').install();

var err = ''

process.argv.forEach((val, index) => {
//   console.log(`${index}: ${val}`);
  if(index != 0 && index != 1) {
        err += val;
}
});


err = err.replace(/__/g, " ")
err = err.replace(/###/g, "\n")



console.log(err)


var eventId = Raven.captureException(err, function (sendErr, eventId) {
    // This callback fires once the report has been sent to Sentry
    if (sendErr) {
      console.error('Failed to send captured exception to Sentry');
    } else {
      console.log('Captured exception and send to Sentry successfully');
    }
});
