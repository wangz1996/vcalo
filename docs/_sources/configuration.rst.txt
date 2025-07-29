Configuration
=============

The configuration file (`default.yaml`) defines the parameters for the simulation.

Example Configuration
---------------------

.. code-block:: yaml

   detector:
       name: calorimeter
       type: electromagnetic
   output:
       file: test.root
       format: ROOT

Parameters
----------

- `detector.name`: The name of the detector.
- `detector.type`: The type of the detector (e.g., electromagnetic, hadronic).
- `output.file`: The output file name.
- `output.format`: The output file format.
