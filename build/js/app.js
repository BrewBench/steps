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

  var notification = null;

  $scope.showSettings = true;
  $scope.error_message = '';

  //default settings values
  $scope.settings = BrewService.settings('settings') || {
    unit: 'F',
    arduinoUrl: '192.168.240.1',
    ports: { 'analog': 5, 'digital': 13 },
    notifications: { on: true, slack: 'Slack notification webhook Url', last: '' },
    sounds: { on: true, alert: '/assets/audio/bike.mp3', timer: '/assets/audio/school.mp3' }
  };

  $scope.steps = BrewService.settings('steps') || [{
    name: 'Step',
    pin: 2,
    analog: false,
    running: false,
    finished: false,
    seconds: 5
  }];

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

  $scope.addStep = function () {
    $scope.steps.push({
      name: 'Next Step',
      pin: 2,
      analog: false,
      running: false,
      finished: false,
      seconds: 5
    });
  };

  $scope.pinInUse = function (pin, analog) {
    return _.find($scope.steps, function (step) {
      return step.analog === analog && step.pin === pin;
    });
  };

  $scope.getPortRange = function (number) {
    number++;
    return Array(number).fill().map(function (_, idx) {
      return 0 + idx;
    });
  };

  $scope.pinChange = function (old_pin, new_pin, analog) {
    //find step with new pin and replace it with old pin
    var step = $scope.pinInUse(new_pin, analog);
    if (step && step.pin == new_pin) {
      step.pin = old_pin;
    }
  };

  $scope.alert = function (step, timer) {

    //don't start alerts until we have hit the temp.target
    if (!timer && step && !step.temp.hit || $scope.settings.notifications.on === false) {
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
          if (step) notification = new Notification(step.name, { body: message, icon: icon });else notification = new Notification('Test kettle', { body: message, icon: icon });
        }
      } else if (Notification.permission !== 'denied') {
        Notification.requestPermission(function (permission) {
          // If the user accepts, let's create a notification
          if (permission === "granted") {
            if (message) {
              notification = new Notification(step.name, { body: message, icon: icon });
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

    arduinoWrite: function arduinoWrite(analog, sensor, value) {
      var q = $q.defer();
      var url = this.domain() + '/arduino/' + (analog ? 'analog' : 'digital') + '/' + sensor + '/' + value;

      $http({ url: url, method: 'GET' }).then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    },

    arduinoRead: function arduinoRead(analog, sensor, timeout) {
      var q = $q.defer();
      var url = this.domain() + '/arduino/' + (analog ? 'analog' : 'digital') + '/' + sensor;

      $http({ url: url, method: 'GET' }).then(function (response) {
        q.resolve(response.data);
      }, function (err) {
        q.reject(err);
      });
      return q.promise;
    }

  };
});