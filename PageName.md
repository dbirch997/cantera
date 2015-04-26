# Introduction #

Cantera is a collection of object-oriented software tools for problems involving chemical kinetics, thermodynamics, and transport processes. Among other things, it can be used to conduct kinetics simulations with large reaction mechanisms, to compute chemical equilibrium, to evaluate thermodynamic and transport properties of mixtures, to evaluate species chemical production rates, to conduct reaction path analysis, to
create process simulators using networks of stirred reactors, and to model non-ideal fluids. Cantera is still in development, and more capabilities continue to be added.


# Typical Applications #
Cantera can be used in many different ways. Here are a few.

**Use it in your own reacting-flow codes.**
> Cantera can be used in Fortran or C++ reacting-flow simulation codes to evaluate properties and chemical source terms that appear in the governing equations. Cantera places no limits on the size of a reaction mechanism, or on the number of mechanisms that you can work with at one time; it can compute transport properties using a full multicomponent formulation; and uses fast, efficient numerical algorithms. It even lets you switch reaction mechanisms or transport property models dynamically during a simulation, to adaptively switch between inexpensive/approximate models and expensive/accurate ones based on local flow conditions.

> It is well-suited for numerical models of laminar flames, flow reactors, chemical vapor deposition reactors, fuel cells, engines, combustors, etc. Any existing code that spends most of its time evaluating kinetic rates (a common situation when large reaction mechanisms are used) may run substantially faster if ported to Cantera. (Cantera’s kinetics algorithm, in particular, runs anywhere from two to four times faster, depending on the platform, than that used in some other widely-used packages.)

**Use it for exploratory calculations.**

> Sometimes you just need a quick answer to a simple question, for example:

  * if air is heated to 3000 K suddenly, how much NO is produced in 1 sec?
  * What is the adiabatic flame temperature of a stoichiometric acetylene/air flame?
  * What are the principal reaction paths in silane pyrolysis?

> With Cantera, answering any of these requires only writing a few lines of code. If you are comfortable with Fortran or C++ you can use either of these, or you can write a short Python script, which has the advantage that it can be run immediately without compilation. Python can also be used interactively. Or you can use one of the stand-alone applications that come with Cantera, which requires no programming at all, other than writing an input file.

**Use it in teaching.**

> Cantera is ideal for use in teaching courses in combustion, reaction engineering, transport processes, kinetics, or similar areas. Every student can have his or her own copy and use it from whatever language or application he or she prefers. There are no issues of cost, site licenses, license managers, etc., as there are for most commercial packages. For this reason, Cantera-based applications are also a good choice for software that accompanies textbooks in these fields.

**Run simulations with your own kinetics, transport, or thermodynamics models.**

> Cantera is designed to be customized and extended. You are not locked in to using a particular equation of state, or reaction rate expressions, or anything else. If you need a different kinetics model than one provided, you can write your own and link it in. The same goes for transport models, equations of state, etc. [this requires C++ programming](Currently.md)

**Make reaction path diagrams and movies.**

> One of the best ways to obtain insight into the most important chemical pathways in a complex reaction mechanism is to make a reaction path diagram, showing the fluxes of a conserved element through the species due to chemistry. But producing these by hand is a slow, tedious process. Cantera can automatically generate reaction path diagrams at any time during a simulation. It is even possible to create reaction path diagram movies, showing how the chemical pathways change with time, as reactants are depleted and products are formed.

**Create stirred reactor networks.**

> Cantera implements a general-purpose stirred reactor model that can be linked to other ones in a network. Reactors can have any number of inlets and outlets, can have a time-dependent volume, and can be connected through devices that regulate the flow rate between them in various ways. Closed-loop controllers may be installed to regulate pressure, temperature, or other properties.Using these basic components, you can build models of a wide variety of systems, ranging in complexity from simple constant-pressure or constant-volume reactors to complete engine simulators.

**Simulate multiphase pure fluids.**

> Cantera implements several accurate equations of state for pure fluids, allowing you to compute properties in the liquid, vapor, mixed liquid/vapor, and supercritical states.

**Simulate Multispecies non-ideal phases**

> Cantera has recently been expanded to include multiphase non-ideal mixture models. A full pitzer implementation of aqueous thermodynamics has been included in the latest release.

**Other**

> These capabilities are only the beginning. Some new features scheduled to be implemented in an upcoming release are non-ideal, multiphase reacting mixtures, surface and interfacial chemistry, sensitivity analysis, models for simple reacting flows ( laminar flames, boundary layers, flow in channels, etc.), and electrochemistry. Other capabilities, or interfaces to other applications, may also be added, depending on available time, support, etc.

> If you have specific things you want to see Cantera support, you are encouraged to become a Cantera developer, or support Cantera development in other ways to make it possible.