'use strict';

angular.module('brewbench-steps', ['ui.router', 'ngTouch', 'duScroll']).config(function ($stateProvider, $urlRouterProvider, $httpProvider) {

  $httpProvider.defaults.useXDomain = true;
  $httpProvider.defaults.headers.common = 'Content-Type: application/json';
  delete $httpProvider.defaults.headers.common['X-Requested-With'];

  $stateProvider.state('home', {
    url: '',
    templateUrl: 'views/home.html',
    controller: 'mainCtrl'
  }).state('otherwise', {
    url: '*path',
    templateUrl: 'views/not-found.html'
  });
});
'use strict';

angular.module('brewbench-steps').controller('mainCtrl', function ($scope, $stateParams, $state, $filter, $timeout, $interval, $q, BrewService) {

  var notification = null,
      timeout = null;

  $scope.showSettings = true;
  $scope.error_message = '';

  //default settings values
  $scope.settings = BrewService.settings('settings') || {
    unit: 'F',
    arduinoUrl: '192.168.240.1',
    ports: { 'analog': 5, 'digital': 13 },
    retrySeconds: 2,
    notifications: { on: true, slack: 'Slack notification webhook Url', last: '' },
    sounds: { on: true, alert: '/assets/audio/bike.mp3', timer: '/assets/audio/school.mp3' }
  };

  $scope.steps = BrewService.settings('steps') || [{
    name: 'Step',
    pin: 2,
    type: 'analog',
    running: false,
    finished: false,
    disabled: false,
    trying: false,
    seconds: 5,
    resetSeconds: 5
  }];

  $scope.changeType = function (step) {
    if (step.type === 'analog') step.type = 'digital';else if (step.type === 'digital') step.type = 'delay';else if (step.type === 'delay') step.type = 'analog';
  };

  $scope.showSettingsSide = function () {
    $scope.showSettings = !$scope.showSettings;
  };

  $scope.clearSteps = function (e, i) {
    angular.element(e.target).html('Removing...');
    BrewService.clear();
    $timeout(function () {
      window.location.reload();
    }, 1000);
  };

  $scope.resetSteps = function () {
    $scope.steps = BrewService.settings('steps');
    _.each($scope.steps, function (step) {
      step.seconds = step.resetSeconds || step.seconds;
      step.finished = false;
      step.running = false;
    });
  };

  $scope.addStep = function () {
    $scope.steps.push({
      name: 'Next Step',
      pin: 2,
      type: 'analog',
      running: false,
      finished: false,
      disabled: false,
      trying: false,
      seconds: 5,
      resetSeconds: 5
    });
  };

  $scope.pinInUse = function (pin, type) {
    return _.find($scope.steps, function (step) {
      return step.type === type && step.pin === pin;
    });
  };

  $scope.getPortRange = function (number) {
    number++;
    return Array(number).fill().map(function (_, idx) {
      return 0 + idx;
    });
  };

  $scope.alert = function (step, timer) {

    //don't start alerts until we have hit the temp.target
    if ($scope.settings.notifications.on === false) {
      return;
    }

    // Desktop / Slack Notification
    var message,
        icon = '/assets/img/brewbench-logo.png',
        color = 'good';

    if (step) {
      icon = '/assets/img/water.png';
      if (!!step.finished) {
        message = step.name + ' is finished';
      } else if (step.length) {
        message = 'All Steps have finished';
      }
    } else {
      message = 'Testing Alerts';
    }

    // Mobile Vibrate Notification
    if ("vibrate" in navigator) {
      navigator.vibrate([500, 300, 500]);
    }

    // Sound Notification
    if ($scope.settings.sounds.on === true) {
      var snd = new Audio(!!timer ? $scope.settings.sounds.timer : $scope.settings.sounds.alert); // buffers automatically when created
      snd.play();
    }

    // Window Notification
    if ("Notification" in window) {
      //close the previous notification
      if (notification) notification.close();

      if (Notification.permission === "granted") {
        if (message) {
          if (step) notification = new Notification(step.name ? step.name : 'Finished', { body: message, icon: icon });else notification = new Notification('Test kettle', { body: message, icon: icon });
        }
      } else if (Notification.permission !== 'denied') {
        Notification.requestPermission(function (permission) {
          // If the user accepts, let's create a notification
          if (permission === "granted") {
            if (message) {
              notification = new Notification(step.name ? step.name : 'Finished', { body: message, icon: icon });
            }
          }
        });
      }
    }
    // Slack Notification
    if ($scope.settings.notifications.slack.indexOf('http') !== -1) {
      BrewService.slack($scope.settings.notifications.slack, message, color, icon, kettle).then(function (response) {
        // console.log('Slack',response);
      });
    }
  };

  $scope.changeSeconds = function (step, seconds) {
    if (seconds >= 0) {
      step.seconds = seconds;
      step.resetSeconds = seconds;
    }
    if (seconds === 0) {
      step.disabled = true;
    }
  };

  $scope.stepRun = function ($index) {
    var step = $scope.steps[$index];
    return $interval(function () {
      //cancel interval if zero out
      if (step.seconds === 0) {
        //stop running
        step.finished = true;
        $scope.startStop($index);
      } else if (step.seconds > 0) {
        //count down seconds
        step.seconds--;
      }
    }, 1000);
  };

  $scope.startStop = function ($index) {
    var step = $scope.steps[$index];
    if (!step) return;else if (step.trying) {
      step.trying = false;
      return;
    }

    var start = step.running ? 0 : 1; //if running then stop

    if (step.type === 'delay') {
      if (start) {
        step.running = true;
        //start timer
        step.interval = $scope.stepRun($index);
      } else {
        //stop timer
        step.running = false;
        $interval.cancel(step.interval);
        //if all timers are done send an alert
        if (_.filter($scope.steps, { finished: true, disabled: false }).length == $scope.steps.length - _.filter($scope.steps, { disabled: true }).length) {
          $scope.alert($scope.steps, true);
        }
        //start next step if there is one
        else if (_.filter($scope.steps, { finished: false, disabled: false }).length) {
            var $nextIndex = _.findIndex($scope.steps, { finished: false, disabled: false });
            if ($nextIndex) $scope.startStop($nextIndex);
          }
      }
    } else {
      step.trying = true;
      //wait for the step relay to stop
      BrewService.arduinoWrite(step.type, step.pin, start).then(function (response) {
        //cancel timeout if we connect
        if (timeout) $timeout.cancel(timeout);
        step.trying = false;
        $scope.error_message = '';

        if (start) {
          step.running = true;
          //start timer
          step.interval = $scope.stepRun($index);
        } else {
          //stop timer
          step.running = false;
          $interval.cancel(step.interval);
          //if all timers are done send an alert
          if (_.filter($scope.steps, { finished: true, disabled: false }).length == $scope.steps.length - _.filter($scope.steps, { disabled: true }).length) {
            $scope.alert($scope.steps, true);
          }
          //start next step if there is one
          else if (_.filter($scope.steps, { finished: false, disabled: false }).length) {
              var $nextIndex = _.findIndex($scope.steps, { finished: false, disabled: false });
              if ($nextIndex) $scope.startStop($nextIndex);
            }
        }
      }, function (err) {
        if (err && typeof err == 'string') $scope.error_message = err;else $scope.error_message = 'Could not connect to the Arduino at ' + BrewService.domain();
        // retry
        if (!timeout) {
          timeout = $timeout(function () {
            $scope.startStop($index);
          }, $scope.settings.retrySeconds * 1000);
        }
      });
    }
  };

  $scope.loadConfig = function () {
    var config = [];
    if (!$scope.pkg) {
      config.push(BrewService.pkg().then(function (response) {
        $scope.pkg = response;
        return $scope.settings.sketch_version = response.sketch_version;
      }));
    }
  };

  $scope.loadConfig();

  // scope watch
  $scope.$watch('settings', function (newValue, oldValue) {
    BrewService.settings('settings', newValue);
  }, true);

  $scope.$watch('steps', function (newValue, oldValue) {
    BrewService.settings('steps', newValue);
  }, true);
});
'use strict';

angular.module('brewbench-steps').directive('editable', function () {
    return {
        restrict: 'E',
        scope: { model: '=', type: '@?', trim: '@?', change: '&?' },
        replace: false,
        template: '<span>' + '<input type="{{type}}" ng-model="model" ng-show="edit" ng-enter="edit=false" ng-change="{{change||false}}" class="editable"></input>' + '<span class="editable" ng-show="!edit">{{(trim) ? (model | limitTo:trim)+"..." : model}}</span>' + '</span>',
        link: function link(scope, element, attrs) {
            scope.edit = false;
            scope.type = !!scope.type ? scope.type : 'text';
            element.bind('click', function () {
                scope.$apply(scope.edit = true);
            });
        }
    };
}).directive('ngEnter', function () {
    return function (scope, element, attrs) {
        element.bind('keypress', function (e) {
            if (e.charCode === 13 || e.keyCode === 13) {
                scope.$apply(attrs.ngEnter);
                if (scope.change) scope.$apply(scope.change);
            }
        });
    };
}).directive('onReadFile', function ($parse) {
    return {
        restrict: 'A',
        scope: false,
        link: function link(scope, element, attrs) {
            var fn = $parse(attrs.onReadFile);

            element.on('change', function (onChangeEvent) {
                var reader = new FileReader();
                var file = (onChangeEvent.srcElement || onChangeEvent.target).files[0];
                var extension = file.name.split('.').pop().toLowerCase();

                reader.onload = function (onLoadEvent) {
                    scope.$apply(function () {
                        fn(scope, { $fileContent: onLoadEvent.target.result, $ext: extension });
                    });
                };
                reader.readAsText(file);
            });
        }
    };
});
'use strict';

angular.module('brewbench-steps').filter('moment', function () {
  return function (date) {
    if (!date) return '';
    return moment(new Date(date)).fromNow();
  };
}).filter('formatDegrees', function ($filter) {
  return function (temp, unit) {
    if (unit == 'F') return $filter('toFahrenheit')(temp);else return $filter('toCelsius')(temp);
  };
}).filter('toFahrenheit', function () {
  return function (celsius) {
    return Math.round(celsius * 9 / 5 + 32);
  };
}).filter('toCelsius', function () {
  return function (fahrenheit) {
    return Math.round((fahrenheit - 32) * 5 / 9);
  };
}).filter('titlecase', function () {
  return function (input) {
    var smallWords = /^(a|an|and|as|at|but|by|en|for|if|in|nor|of|on|or|per|the|to|vs?\.?|via)$/i;

    input = input.toLowerCase();
    return input.replace(/[A-Za-z0-9\u00C0-\u00FF]+[^\s-]*/g, function (match, index, title) {
      if (index > 0 && index + match.length !== title.length && match.search(smallWords) > -1 && title.charAt(index - 2) !== ":" && (title.charAt(index + match.length) !== '-' || title.charAt(index - 1) === '-') && title.charAt(index - 1).search(/[^\s-]/) < 0) {
        return match.toLowerCase();
      }

      if (match.substr(1).search(/[A-Z]|\../) > -1) {
        return match;
      }

      return match.charAt(0).toUpperCase() + match.substr(1);
    });
  };
});
'use strict';

angular.module('brewbench-steps').factory('BrewService', function ($http, $q, $filter) {
  return {
    clear: function clear() {
      if (window.localStorage) {
        window.localStorage.removeItem('settings');
        window.localStorage.removeItem('steps');
        window.localStorage.removeItem('urls');
      }
    },

    settings: function settings(key, values) {
      if (!window.localStorage) return values;
      try {
        if (values) {
          return window.localStorage.setItem(key, JSON.stringify(values));
        } else if (window.localStorage.getItem(key)) {
          return JSON.parse(window.localStorage.getItem(key));
        }
      } catch (e) {
        /*JSON parse error*/
      }
      return values;
    },

    domain: function domain(format) {
      var settings = this.settings('settings');
      var domain = '';

      if (settings && settings.arduinoUrl) domain = settings.arduinoUrl.indexOf('//') === -1 ? '//' + settings.arduinoUrl : settings.arduinoUrl;else if (document.location.host == 'localhost') domain = '//arduino.local';

      if (!!format) return domain.indexOf('//') !== -1 ? domain.substring(domain.indexOf('//') + 2) : domain;
      return domain;
    },

    slack: function slack(webhook_url, msg, color, icon, step) {
      var q = $q.defer();

      var postObj = { 'attachments': [{ 'fallback': msg,
          'title': step.name,
          'title_link': 'http://' + document.location.host + '/#/arduino/' + this.domain(true),
          'fields': [{ 'value': msg }],
          'color': color,
          'mrkdwn_in': ['text', 'fallback', 'fields'],
          'thumb_url': icon
        }]
      };

      $http({ url: webhook_url, method: 'POST', data: 'payload=' + JSON.stringify(postObj), headers: { 'Content-Type': 'application/x-www-form-urlencoded' } }).then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    },

    pkg: function pkg() {
      var q = $q.defer();
      $http.get('/package.json').then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    },

    arduinoWrite: function arduinoWrite(type, sensor, value) {
      var q = $q.defer();
      var url = this.domain() + '/arduino/' + type + '/' + sensor + '/' + value;

      $http({ url: url, method: 'GET' }).then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    },

    arduinoRead: function arduinoRead(type, sensor, timeout) {
      var q = $q.defer();
      var url = this.domain() + '/arduino/' + type + '/' + sensor;

      $http({ url: url, method: 'GET' }).then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    }

  };
});