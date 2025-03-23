const express = require('express');
const router = express.Router();
const hydroponics = require('../services/hydroponicsService');

router.get('/push/:name/:value',hydroponics.push);
router.get('/', hydroponics.fetch);
router.get('/refresh', hydroponics.refresh);

module.exports = router;
