angular.module('brewbench-steps')
.filter('moment', function() {
  return function(date) {
      if(!date)
        return '';
      return moment(new Date(date)).fromNow();
    };
})
.filter('formatDegrees', function($filter) {
  return function(temp,unit) {
    if(unit=='F')
      return $filter('toFahrenheit')(temp);
    else
      return $filter('toCelsius')(temp);
  };
})
.filter('toFahrenheit', function() {
  return function(celsius) {
    return Math.round(celsius*9/5+32);
  };
})
.filter('toCelsius', function() {
  return function(fahrenheit) {
    return Math.round((fahrenheit-32)*5/9);
  };
})
.filter('titlecase', function() {
    return function (input) {
        var smallWords = /^(a|an|and|as|at|but|by|en|for|if|in|nor|of|on|or|per|the|to|vs?\.?|via)$/i;

        input = input.toLowerCase();
        return input.replace(/[A-Za-z0-9\u00C0-\u00FF]+[^\s-]*/g, function(match, index, title) {
            if (index > 0 && index + match.length !== title.length &&
                match.search(smallWords) > -1 && title.charAt(index - 2) !== ":" &&
                (title.charAt(index + match.length) !== '-' || title.charAt(index - 1) === '-') &&
                title.charAt(index - 1).search(/[^\s-]/) < 0) {
                return match.toLowerCase();
            }

            if (match.substr(1).search(/[A-Z]|\../) > -1) {
                return match;
            }

            return match.charAt(0).toUpperCase() + match.substr(1);
        });
    }
});
