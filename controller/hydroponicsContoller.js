const express = require('express');
const router = express.Router();
const hydroponics = require('../services/hydroponicsService');

router.get('/push/:name/:value',hydroponics.push);
router.get('/', hydroponics.fetch);
router.get('/refresh', hydroponics.refresh);
router.get('/NutrientLevel', hydroponics.nurtrients);
router.get('/GrowLED', hydroponics.grow_led);
router.get('/OxygenMotor', hydroponics.o2_motor);
router.get('/WaterPump', hydroponics.water_pump);
module.exports = router;
