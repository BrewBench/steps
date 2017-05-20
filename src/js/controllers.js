angular.module('brewbench-steps')
.controller('mainCtrl', function($scope, $stateParams, $state, $filter, $timeout, $interval, $q, BrewService){

  var notification = null;

  $scope.showSettings = true;
  $scope.error_message = '';

  //default settings values
  $scope.settings = BrewService.settings('settings') || {
    unit: 'F'
    ,arduinoUrl: '192.168.240.1'
    ,ports: {'analog':5, 'digital':13}
    ,notifications: {on:true,slack:'Slack notification webhook Url',last:''}
    ,sounds: {on:true,alert:'/assets/audio/bike.mp3',timer:'/assets/audio/school.mp3'}
  };

  $scope.steps = BrewService.settings('steps') || [{
    name: 'Step'
    ,pin: 2
    ,analog: false
    ,running: false
    ,finished: false
    ,seconds: 5
  }];

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

  $scope.addStep = function(){
      $scope.steps.push({
          name: 'Next Step'
          ,pin: 2
          ,analog: false
          ,running: false
          ,finished: false
          ,seconds: 5
        });
  };

  $scope.pinInUse = function(pin,analog){
    return _.find($scope.steps, function(step){
      return step.analog === analog && step.pin === pin;
    });
  };

  $scope.getPortRange = function(number){
      number++;
      return Array(number).fill().map((_, idx) => 0 + idx);
  };

  $scope.pinChange = function(old_pin,new_pin,analog){
    //find step with new pin and replace it with old pin
    var step = $scope.pinInUse(new_pin,analog);
    if(step && step.pin == new_pin){
        step.pin = old_pin;
    }
  };

  $scope.alert = function(step,timer){

    //don't start alerts until we have hit the temp.target
    if(!timer && step && !step.temp.hit
    || $scope.settings.notifications.on===false){
      return;
    }

    // Desktop / Slack Notification
    var message, icon = '/assets/img/brewbench-logo.png', color = 'good';

    if(step){
      icon = '/assets/img/water.png';
      if(!!step.finished){
        message = step.name+' is finished';
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
            notification = new Notification(step.name,{body:message,icon:icon});
          else
            notification = new Notification('Test kettle',{body:message,icon:icon});
        }
      } else if(Notification.permission !== 'denied'){
        Notification.requestPermission(function (permission) {
          // If the user accepts, let's create a notification
          if (permission === "granted") {
            if(message){
              notification = new Notification(step.name,{body:message,icon:icon});
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

  // scope watch
  $scope.$watch('settings',function(newValue,oldValue){
    BrewService.settings('settings',newValue);
  },true);

  $scope.$watch('steps',function(newValue,oldValue){
    BrewService.settings('steps',newValue);
  },true);

});
