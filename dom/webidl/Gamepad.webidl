/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

interface nsIVariant;

interface Gamepad {
  /**
   * An identifier, unique per type of device.
   */
  readonly attribute DOMString id;

  /**
   * The game port index for the device. Unique per device
   * attached to this system.
   */
  readonly attribute unsigned long index;

  /**
   * true if this gamepad is currently connected to the system.
   */
  readonly attribute boolean connected;

  /**
   * The current state of all buttons on the device, an
   * array of doubles.
   */
  [Throws]
  readonly attribute nsIVariant buttons;

  /**
   * The current position of all axes on the device, an
   * array of doubles.
   */
  [Throws]
  readonly attribute nsIVariant axes;
};
