angular.module('brewbench-steps')
.controller('mainCtrl', function($scope, $stateParams, $state, $filter, $timeout, $interval, $q, BrewService){

  var notification = null, timeout = null;

  $scope.showSettings = true;
  $scope.error_message = '';
  $scope.pkg='';

  //default settings values
  $scope.settings = BrewService.settings('settings') || {
    unit: 'F'
    ,arduinoUrl: '192.168.240.1'
    ,ports: {'analog':5, 'digital':13}
    ,recipe: {'name':''}
    ,retrySeconds: 2
    ,notifications: {on:true,slack:'Slack notification webhook Url',last:''}
    ,sounds: {on:true,alert:'/assets/audio/bike.mp3',timer:'/assets/audio/school.mp3'}
  };

  $scope.steps = BrewService.settings('steps') || [{
    name: 'Step'
    ,pin: 2
    ,type: 'analog'
    ,group: 'Step Group'
    ,run: 'start/stop'
    ,running: false
    ,finished: false
    ,disabled: false
    ,trying: false
    ,seconds: 5
    ,resetSeconds: 5
  }];

  $scope.changeType = function(step){
    if(step.type === 'analog')
      step.type = 'digital';
    else if(step.type === 'digital')
      step.type = 'delay';
    else if(step.type === 'delay')
      step.type = 'analog';
  };

  $scope.changeRun = function(step){
    if(step.run === 'start')
      step.run = 'stop';
    else if(step.run === 'stop')
      step.run = 'start/stop';
    else if(step.run === 'start/stop')
      step.run = 'start';
  };

  $scope.showSettingsSide = function(){
    $scope.showSettings = !$scope.showSettings;
  };

  $scope.clearSteps = function(e,i){
      angular.element(e.target).html('Removing...');
      BrewService.clear();
      $timeout(function(){
        window.location.reload();
      },1000);
  };

  $scope.resetSteps = function(){
    $scope.steps = BrewService.settings('steps');
    _.each($scope.steps, function(step){
      step.seconds = step.resetSeconds || step.seconds;
      step.finished = false;
      step.running = false;
    });
  };

  $scope.addStep = function(step){
      if(!step) step = {};
      var newStep = {
                      name: 'Next Step'
                      ,pin: 2
                      ,type: 'analog'
                      ,group: 'Step Group'
                      ,run: 'start/stop'
                      ,running: false
                      ,finished: false
                      ,disabled: false
                      ,trying: false
                      ,seconds: 5
                      ,resetSeconds: 5
                    };
      if(step.seconds && !step.resetSeconds)
        step.resetSeconds = step.seconds;
      $scope.steps.push(_.merge(newStep, step));
  };

  $scope.pinInUse = function(pin,type){
    return _.find($scope.steps, function(step){
      return step.type === type && step.pin === pin;
    });
  };

  $scope.getPortRange = function(number){
      number++;
      return Array(number).fill().map((_, idx) => 0 + idx);
  };

  $scope.alert = function(step,timer){

    //don't start alerts until we have hit the temp.target
    if($scope.settings.notifications.on===false){
      return;
    }

    // Desktop / Slack Notification
    var message, icon = '/assets/img/brewbench-logo.png', color = 'good';

    if(step){
      icon = '/assets/img/water.png';
      if(!!step.finished){
        message = step.name+' is finished';
      } else if(step.length){
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
    if($scope.settings.sounds.on===true){
      var snd = new Audio((!!timer) ? $scope.settings.sounds.timer : $scope.settings.sounds.alert); // buffers automatically when created
      snd.play();
    }

    // Window Notification
    if("Notification" in window){
      //close the previous notification
      if(notification)
        notification.close();

      if(Notification.permission === "granted"){
        if(message){
          if(step)
            notification = new Notification(step.name ? step.name : 'Finished',{body:message,icon:icon});
          else
            notification = new Notification('Test kettle',{body:message,icon:icon});
        }
      } else if(Notification.permission !== 'denied'){
        Notification.requestPermission(function (permission) {
          // If the user accepts, let's create a notification
          if (permission === "granted") {
            if(message){
              notification = new Notification(step.name ? step.name : 'Finished',{body:message,icon:icon});
            }
          }
        });
      }
    }
    // Slack Notification
    if($scope.settings.notifications.slack.indexOf('http')!==-1){
      BrewService.slack($scope.settings.notifications.slack,message,color,icon,kettle).then(function(response){
        // console.log('Slack',response);
      });
    }
  };

  $scope.changeSeconds = function(step,seconds){
    if(seconds >= 0){
      step.seconds = seconds;
      step.resetSeconds = seconds;
    }
    if(seconds === 0){
      step.disabled = true;
    }
  };

  $scope.stepRun = function($index){
    var step = $scope.steps[$index];
    return $interval(function () {
      //cancel interval if zero out
      if(step.seconds === 0){
        //stop running
        step.finished = true;
        $scope.startStop($index);
      } else if(step.seconds > 0){
        //count down seconds
        step.seconds--;
      }
    },1000);
  };

  $scope.startStop = function($index){
    var step = $scope.steps[$index];
    if(!step)
      return;
    else if(step.trying){
      step.trying = false;
      return;
    }

    function finishedSteps(){
      //if all timers are done send an alert
      if(_.filter($scope.steps, {finished: true, disabled: false}).length
        == ($scope.steps.length - _.filter($scope.steps, {disabled: true}).length) ){
        $scope.alert($scope.steps,true);
      }
    }

    function startNextStep(){
      if(_.filter($scope.steps, {finished: false, disabled: false}).length){
        var $nextIndex = _.findIndex($scope.steps, {finished: false, disabled: false});
        if($nextIndex)
          $scope.startStop($nextIndex);
      }
    }

    var start = (step.running) ? 0 : 1; //if running then stop

    if(step.type === 'delay'){
      if(start){
        step.running = true;
        //start timer
        step.interval = $scope.stepRun($index);
      } else {
        //stop timer
        step.running = false;
        $interval.cancel(step.interval);
        finishedSteps();
        startNextStep();
      }
    } else {
      step.trying = true;

      // is this a start or stop step that finished?
      if(step.run !== 'start/stop' && !!step.finished){
        step.running = false;
        step.trying = false;
        $interval.cancel(step.interval);
        finishedSteps();
        return startNextStep();
      } else if(step.run === 'start' && !step.finished && !step.running){
        // is this a start step?
        start = 1;
      } else if(step.run === 'stop' && !step.finished && !step.running){
        // is this a stop step?
        start = 0;
      }

      //wait for the step relay to stop
      BrewService.arduinoWrite(step.type, step.pin, start).then(function(response){
        //cancel timeout if we connect
        if(timeout)
          $timeout.cancel(timeout);
        step.trying = false;
        $scope.error_message = '';

        if(!!start || step.run === 'stop'){
          step.running = true;
          //start timer
          step.interval = $scope.stepRun($index);
        } else {
          //stop timer
          step.running = false;
          $interval.cancel(step.interval);
          finishedSteps();
          startNextStep();
        }
      },function(err){
        if(err && typeof err == 'string')
          $scope.error_message = err;
        else
          $scope.error_message = 'Could not connect to the Arduino at '+BrewService.domain();
        // retry
        if(!timeout){
          timeout = $timeout(function(){
            $scope.startStop($index);
          },$scope.settings.retrySeconds*1000);
        }
      });
    }
  };

  // restart step if it was running
  $scope.init = function(){
    if(!!$scope.steps){
      _.each($scope.steps, function(step, $index){
        if(step.running){
          step.interval = $scope.stepRun($index);
        }
      });
    }
  };

  $scope.loadConfig = function(){
    var config = [];
    if(!$scope.pkg){
      config.push(BrewService.pkg().then(function(response){
          $scope.pkg = response;
        })
      );
    }
    return $q.all(config);
  };

  $scope.importSteps = function($fileContent,$ext){
    var importContent = YAML.parse($fileContent);
    if(importContent){
      if(importContent.name)
      $scope.settings.recipe.name = importContent.name;
      $scope.steps = [];
      if(importContent.steps){
        _.each(importContent.steps, function(step){
          $scope.addStep(step);
        });
      }
      if(importContent.groups){
        _.each(importContent.groups, function(group){
          _.each(group.steps, function(step){
            step.group = group.name;
            $scope.addStep(step);
          });
        });
      }
      $scope.recipe_success = true;
    } else {
      $scope.recipe_success = false;
    }
  };

  $scope.loadConfig()
    .then($scope.init);

  // scope watch
  $scope.$watch('settings',function(newValue,oldValue){
    BrewService.settings('settings',newValue);
  },true);

  $scope.$watch('steps',function(newValue,oldValue){
    BrewService.settings('steps',newValue);
  },true);

});
